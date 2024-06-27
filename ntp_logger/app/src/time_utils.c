/**
 * Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#include <gio/gio.h>
#include <glib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <fapp/logging.h>
#include <time_utils.h>

#define NTP_STATUS_LINES 13

/* Run the chronyc tracking command to get the NTP status.
 *
 * Returns a GString that should be freed by the caller.
 */
GString *time_utils_get_ntp_status_info(GError **error) {
  // The format of the output is:
  // chronyc tracking
  //    Reference ID    : XXXXXXXX (<URL>)
  //    Stratum         : 2
  //    Ref time (UTC)  : Wed Apr 24 15:27:12 2024
  //    System time     : 0.001420902 seconds slow of NTP time
  //    Last offset     : -0.001718123 seconds
  //    RMS offset      : 0.000821102 seconds
  //    Frequency       : 2.809 ppm slow
  //    Residual freq   : -2.628 ppm
  //    Skew            : 0.847 ppm
  //    Root delay      : 0.023985652 seconds
  //    Root dispersion : 0.000909695 seconds
  //    Update interval : 65.0 seconds
  //    Leap status     : Normal

  // Note: this is expected to fail in older FW, such as LTS 2020,
  // where chronyc is not available.
  GSubprocess *subprocess = g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE,
                                             NULL, "chronyc", "tracking", NULL);
  if (subprocess == NULL) {
    if (error != NULL) {
      *error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
                           "Failed to create subprocess");
    }
    return NULL;
  }

  GInputStream *stdout_stream =
      g_subprocess_get_stdout_pipe(subprocess); // Owned by subprocess
  GDataInputStream *data_input_stream = g_data_input_stream_new(stdout_stream);

  GString *output = g_string_new(NULL);
  size_t lines_read = 0;
  while (true) {
    // Get next line
    char *line =
        g_data_input_stream_read_line(data_input_stream, NULL, NULL, NULL);
    // If return is NULL, then the stream has ended.
    if (line == NULL) {
      break;
    }
    // Append line to output buffer.
    g_string_append(output, line);
    g_string_append_c(output, '\n');
    g_free(line);
    lines_read++;
  }

  g_object_unref(data_input_stream);
  g_object_unref(subprocess);

  // Check that we got the expected number of lines. Otherwise that might
  // indicate that the command failed, e.g. that chronyc failed to talk with the
  // daemon.
  if (lines_read != NTP_STATUS_LINES) {
    if (error != NULL) {
      *error = g_error_new(G_IO_ERROR, G_IO_ERROR_FAILED,
                           "Too few lines read (%d, output before: '%s')",
                           (int)lines_read, output->str);
    }
    g_string_free(output, TRUE);
    return NULL;
  }

  return output;
}

/* Parse the output from the time_utils_get_ntp_status_info function
 * to get the status and difference.
 *
 * The 'status' should be freed by the caller.
 */
bool time_utils_parse_ntp_status(GString *ntp_info, char **status,
                                 double *difference) {
  // The format of the input is:
  // chronyc tracking
  //    Reference ID    : XXXXXXXX (<URL>)
  //    Stratum         : 2
  //    Ref time (UTC)  : Wed Apr 24 15:27:12 2024
  //    System time     : 0.001420902 seconds slow of NTP time
  //    Last offset     : -0.001718123 seconds
  //    RMS offset      : 0.000821102 seconds
  //    Frequency       : 2.809 ppm slow
  //    Residual freq   : -2.628 ppm
  //    Skew            : 0.847 ppm
  //    Root delay      : 0.023985652 seconds
  //    Root dispersion : 0.000909695 seconds
  //    Update interval : 65.0 seconds
  //    Leap status     : Normal
  // The fourth line is the difference and the 13th line is the status

  // Split the input into lines
  char **lines = g_strsplit(ntp_info->str, "\n", -1);
  if (lines == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to split lines");
    return false;
  }
  size_t num_lines = g_strv_length(lines);

  // Find the interesting lines
  if (num_lines < NTP_STATUS_LINES) {
    fapp_logger_log(LOG_ERR, "Not enough lines in the output");
    g_strfreev(lines);
    return false;
  }

  const char *difference_line = lines[3];
  const char *status_line = lines[12];

  // Parse the difference
  if (difference_line == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to read difference line");
    g_strfreev(lines);
    return false;
  }
  char *difference_str = g_strstr_len(difference_line, -1, ":");
  if (difference_str == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to find difference value");
    g_strfreev(lines);
    return false;
  }
  difference_str += 2;
  *difference = g_ascii_strtod(difference_str, NULL);

  // Parse the status
  if (status_line == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to read status line");
    g_strfreev(lines);
    return false;
  }
  char *status_str =
      g_strstr_len(status_line, -1, ":"); // Data owned by the function
  if (status_str == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to find status value");
    g_strfreev(lines);
    return false;
  }
  status_str += 2;
  *status = g_strdup(status_str);

  g_strfreev(lines);
  return true;
}

/* This returns the UTC time in milliseconds since the epoch
 *
 * This might go backwards if the system time is changed.
 */
uint64_t time_utils_get_system_epoch_time_us() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

/* This returns the time since an arbitrary point (e.g. system boot) in
 * microseconds
 *
 * This will always increase, even if the system time is changed.
 */
uint64_t time_utils_get_monotonic_time_us() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}