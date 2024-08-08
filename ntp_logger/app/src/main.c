/**
 * Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#include <glib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <fapp/logging.h> // Logging to both stdout and system log
#include <fapp/misc.h>    // For signal handling
#include <time_utils.h>
#include <uploader.h>

/* This function loops until exit is requested. In each iteration
 * it will collect all the needed time and NTP data, it will then
 * log it to a temporary file and if it is time for a new upload,
 * it will upload the CSV and status files to the S3 bucket.
 */
bool log_time_difference(const char *output_file, size_t interval_ms) {
  uint64_t last_time = time_utils_get_monotonic_time_us();
  uint64_t last_system_time = time_utils_get_system_epoch_time_us();
  g_usleep(interval_ms * 1000);

  // This stores the NTP status from the last time it was not synced.
  GString *last_failed_ntp_info = NULL;

  // Run until we receive a signal to stop
  while (!fapp_do_stop) {
    // Open or create a file where we can store  the CSV data
    // until it is time to upload it to the server
    FILE *fp = fopen(output_file, "a");
    if (fp == NULL) {
      fapp_logger_log(LOG_ERR, "Failed to open file %s", output_file);
      return false;
    }

    uint64_t current_time = time_utils_get_monotonic_time_us();
    uint64_t current_system_time = time_utils_get_system_epoch_time_us();

    int64_t elapsed_time = current_time - last_time;
    int64_t elapsed_system_time =
        current_system_time - last_system_time; // Might be negative

    double difference;
    char *status = NULL;

    GError *error = NULL;
    // We only want to log the error if this function succeeded last time,
    // to avoid spamming the logs if it is failing continuously.
    static bool succeeded_last_time = true;
    GString *ntp_info = time_utils_get_ntp_status_info(&error);
    if (ntp_info) {
      if (!time_utils_parse_ntp_status(ntp_info, &status, &difference)) {
        fapp_logger_log(LOG_ERR, "Failed to parse NTP status with text:");
        fapp_logger_log(LOG_ERR, "%s", ntp_info->str);
        difference = 0;
        status = g_strdup("Unknown");
      }
      succeeded_last_time = true;
    } else {
      if (error) {
        if (succeeded_last_time) {
          fapp_logger_log(LOG_ERR, "Failed to get NTP info: %s",
                          error->message);
        }
        g_error_free(error);
      }
      difference = 0;
      status = g_strdup("Unknown");
      succeeded_last_time = false;
    }

    // If the status is not normal, store the NTP info for later upload
    if (ntp_info && g_strcmp0(status, "Normal") != 0) {
      if (last_failed_ntp_info) {
        g_string_free(last_failed_ntp_info, TRUE);
      }
      // Copy the GString since it will be freed at the end of this iteration
      // and we want it to be valid until the next time we perform an upload
      last_failed_ntp_info = g_string_new_len(ntp_info->str, ntp_info->len);
    }

    // Append the data from this iteration to the CSV file
    fprintf(fp, "%" PRId64 ",%" PRId64 ",%" PRIu64 ",%f,%s\n", elapsed_time,
            elapsed_system_time, current_system_time, difference, status);
    // Update the last time variables with the current time
    // to prepare them for the next iteration
    last_time = current_time;
    last_system_time = current_system_time;

    g_usleep(interval_ms * 1000);
    // We're closing the file since we will open it for reading when uploading.
    // This is not optimal to do, but performance should not be an issue in this
    // application.
    fclose(fp);

    // Upload the difference log file if the specified duration
    // has passed since last upload
    if (uploader_check_if_time_to_upload()) {
      fapp_logger_log(LOG_INFO, "Uploading files...");

      // Upload the difference log file
      if (!uploader_upload_file(output_file, PRESIGNED_URL_TIME_LOG)) {
        fapp_logger_log(LOG_ERR, "Failed to upload file");
      } else {
        fapp_logger_log(LOG_INFO, "File uploaded successfully");
        // Truncate the file after upload
        if (truncate(output_file, 0) != 0) {
          fapp_logger_log(LOG_ERR, "Failed to truncate file");
        }
      }

      // Upload the complete status
      if (ntp_info) {
        if (!uploader_upload_text(ntp_info->str, PRESIGNED_URL_NTP_STATUS)) {
          fapp_logger_log(LOG_ERR, "Failed to upload NTP status");
        } else {
          fapp_logger_log(LOG_INFO, "NTP status uploaded successfully");
        }
      }

      // If there has been a failed status since last upload, upload it
      if (last_failed_ntp_info) {
        if (!uploader_upload_text(last_failed_ntp_info->str,
                                  PRESIGNED_URL_NTP_FAILED_STATUS)) {
          fapp_logger_log(LOG_ERR, "Failed to upload (failed) NTP status");
        } else {
          fapp_logger_log(LOG_INFO,
                          "NTP status (last failed) uploaded successfully");
        }
        // Unset it so we do not upload it again
        g_string_free(last_failed_ntp_info, TRUE);
        last_failed_ntp_info = NULL;
      }
    }

    if (ntp_info) {
      g_string_free(ntp_info, TRUE);
    }
  }

  return true;
}

int main(int argc, char **argv) {
  fapp_logger_init(BIN_NAME);
  fapp_logger_log(LOG_INFO, "Application started");

  // Setup a signal handler so that we can perform clean-up and
  // return of peripherals when receiving a signal or when an
  // error condition such as segfaults is detected.
  fapp_signal_handler_init();

  // Run main loop until receiving a signal. In the main loop, we will
  // sample the time difference and NTP status every second and log it to a
  // file that is uploaded every UPLOAD_INTERVAL_S seconds. This interval
  // is specified in the fixedit-manifest.json files or as a build argument.
  log_time_difference("/tmp/time_difference.csv", 1000);

  fapp_logger_log(LOG_INFO, "Application exited");
  return EXIT_SUCCESS;
}
