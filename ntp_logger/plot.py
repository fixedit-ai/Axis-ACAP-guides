#
# Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
#

import click
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from pathlib import Path
from tempfile import NamedTemporaryFile
import boto3
from botocore.exceptions import NoCredentialsError
from botocore.exceptions import ClientError

def get_version_by_index(s3, bucket_name, object_key, index):
    # Get all versions of the file
    try:
        versions = s3.list_object_versions(Bucket=bucket_name, Prefix=object_key)["Versions"]
    except ClientError as e:
        error_message = f"Error code {e.response['Error']['Code']}: {e.response['Error']['Message']}"
        print(error_message)
        raise RuntimeError(error_message)

    # Sort versions by the 'LastModified' key in ascending order, so
    # that index 0 is the first version.
    sorted_versions = sorted(versions, key=lambda x: x["LastModified"])

    # Handling negative indices for reverse indexing
    if index < 0:
        index = len(sorted_versions) + index

    # Access the specific version based on the index
    if 0 <= index < len(sorted_versions):
        version_id = sorted_versions[index]["VersionId"]
        return version_id
    else:
        raise IndexError("Version index out of range")


def download_version(bucket_name, object_key, download_path, version_index=0):
    s3 = boto3.client("s3")

    version_id = get_version_by_index(s3, bucket_name, object_key, version_index)

    s3.download_file(
        Bucket=bucket_name,
        Key=object_key,
        Filename=download_path,
        ExtraArgs={"VersionId": version_id},
    )


def plot_dataframe(df, title=None):
    """Plot the clock adjustment and clock diff over system time with status highlighted.

    The time_delta and the clock_diff are assumed to be in seconds as floating point numbers.
    The status_mask is assumed to be 0 for normal and 1 for non-normal status, in case of
    non-normal status, the status is assumed to be a string.
    """
    # Convert curent system time from Unix timestamp in microseconds to datetime
    df["time"] = pd.to_datetime(df["current_system_time"], unit="us")

    # Creating the figure and axes
    fig, axs = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

    # Plotting time_delta
    axs[0].plot(df["time"], df["time_delta"], label="Time Delta", color="blue")
    axs[0].set_title("Clock adjustment: dSys - dMon")
    axs[0].set_ylabel("Adjustment in interval [s]")
    axs[0].plot(df["time"], df["time_delta"], ".", color="blue")

    # Plotting clock_diff
    axs[1].plot(df["time"], df["clock_diff"], label="clock_diff", color="orange")
    axs[1].set_title("Clock difference from NTP server")
    axs[1].set_ylabel("Time difference [s]")
    axs[1].plot(df["time"], df["clock_diff"], ".", color="orange")

    # Highlighting non-normal status
    for _, row in df[df["status_mask"] != 0].iterrows():
        for ax in axs:
            ax.axvline(x=row["time"], color="red", linestyle="--", alpha=0.5)
            ax.text(
                row["time"],
                ax.get_ylim()[1] * 0.9,
                row["status"],
                rotation=90,
                verticalalignment="top",
            )

    # Format the datetime on the x-axis
    for ax in axs:
        ax.xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d %H:%M:%S"))
        ax.xaxis.set_major_locator(mdates.MinuteLocator(interval=10))
        ax.xaxis.set_minor_locator(mdates.MinuteLocator(interval=1))
        ax.set_xlabel("Time (UTC)")
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45)

    # Adding grid and legend
    for ax in axs:
        ax.legend()
        ax.grid(True)

    # Adding title
    if title:
        fig.suptitle(title)

    plt.tight_layout()
    plt.show()


@click.command()
@click.argument("filepath", type=str)
@click.option("--title", type=str, default=None)
@click.option("--version-index", type=int, default=-1)
def plot_time_delta(filepath, title, version_index):
    csv_names = [
        "elapsed_time",
        "elapsed_system_time",
        "current_system_time",
        "clock_diff",
        "status",
    ]

    # If the file path starts with s3://, then download it to a temporary file
    if filepath.startswith("s3://"):
        bucket_name, key = filepath[5:].split("/", 1)
        print(f"Downloading file from s3://{bucket_name}/{key} with version index {version_index}")

        # Download the file to a temporary file
        with NamedTemporaryFile() as tmpfile:
            download_version(
                bucket_name, key, tmpfile.name, version_index=int(version_index)
            )
            tmpfile.seek(0)
            df = pd.read_csv(tmpfile.name, header=None, names=csv_names)
    else:
        df = pd.read_csv(filepath, header=None, names=csv_names)

    # Get a boolean mask for the state where 0 is "Normal"
    df["status_mask"] = df["status"].apply(lambda x: 0 if x == "Normal" else 1)

    # If the status is "Unknown" or "Not synchronised", then we could not get a clock_diff
    # from the NTP server. In the CSV, these are represented as 0, so we should replace
    # them with None.
    clock_diff_mask = df["status"].isin(["Unknown", "Not synchronised"])
    df.loc[clock_diff_mask, "clock_diff"] = None

    # Calculate the time delta and convert it from microseconds to seconds. The clock_diff
    # is already in seconds as floating point numbers.
    df["time_delta"] = df["elapsed_system_time"] - df["elapsed_time"]
    df["time_delta"] = df["time_delta"] / 1_000_000.0

    # Print the date time for start and end of plot
    start_datetime = pd.to_datetime(df["current_system_time"].iloc[0], unit="us")
    end_datetime = pd.to_datetime(df["current_system_time"].iloc[-1], unit="us")
    print(f"Start time (UTC): {start_datetime}")
    print(f"End time (UTC): {end_datetime}")

    # Plotting the time delta
    if title is None:
        title = Path(filepath).name
    plot_dataframe(df, title)


if __name__ == "__main__":
    plot_time_delta()
