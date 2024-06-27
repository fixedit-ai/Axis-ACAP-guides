#
# Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
#

# This script generates a presigned URL that we can upload files to.
# Note that the `aws s3 presign` CLI command is just for GET, not PUT,
# therefore we use this script instead.

import boto3
import click

BUCKET_NAME = "my.bucket"
BUCKET_REGION = "eu-north-1"
MAX_EXPIRATION = 3600 * 24 * 7  # 1 week

FILE_PREFIX = "ntp/dev/"
TIME_LOG_FILE = "time_log.csv"
NTP_STATUS_FILE = "ntp_status.txt"
NTP_FAILED_STATUS_FILE = "ntp_failed_status.txt"


def generate_presigned_url(bucket_name, object_name, expiration=3600):
    s3_client = boto3.client("s3")
    response = s3_client.generate_presigned_url(
        "put_object",
        Params={"Bucket": bucket_name, "Key": object_name},
        ExpiresIn=expiration,
    )
    return response


def generate_and_write_presigned_url(bucket_name, file_prefix, file_name, expiration, output_file):
    print(f"Generate presigned URLs for {bucket_name}/{file_prefix + file_name} with expiration {expiration} seconds")
    url = generate_presigned_url(bucket_name, file_prefix + file_name, expiration=expiration)
    env_var_name = file_name.upper().replace(".", "_")
    output_file.write(f"export {env_var_name}='{url}'\n")
    print(f"Writing URL to {output_file.name}")


@click.command()
@click.option("--bucket", default=BUCKET_NAME, help="S3 bucket name")
@click.option("--file-prefix", default=FILE_PREFIX, help="File prefix")
@click.option("--expiration", default=MAX_EXPIRATION, help="Expiration time in seconds", type=click.IntRange(1, MAX_EXPIRATION))
@click.option("--output", default="s3.env", help="Output file where the URLs are written")
def main(bucket, file_prefix, expiration, output):
    with open(output, "w") as output_file:
        generate_and_write_presigned_url(bucket, file_prefix, TIME_LOG_FILE, expiration, output_file)
        generate_and_write_presigned_url(bucket, file_prefix, NTP_STATUS_FILE, expiration, output_file)
        generate_and_write_presigned_url(bucket, file_prefix, NTP_FAILED_STATUS_FILE, expiration, output_file)


if __name__ == "__main__":
    main()