/**
 * Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

// This file is making use of cURL to upload the NTP log CSV file to S3
// using a presigned link.
#include <curl/curl.h>
#include <glib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#include <fapp/logging.h>
#include <time_utils.h>

typedef struct {
  const char *data;
  size_t length;
  size_t read_pos;
} string_upload_info_s;

bool uploader_check_if_time_to_upload() {
  static uint64_t last_upload_time = 0;
  if (last_upload_time == 0) {
    last_upload_time = time_utils_get_system_epoch_time_us();
    return false;
  }

  // Check if it is time to upload
  uint64_t current_time = time_utils_get_system_epoch_time_us();
  if (current_time - last_upload_time > (uint64_t)UPLOAD_INTERVAL_S * 1000000) {
    last_upload_time = current_time;
    return true;
  }
  return false;
}

// Callback function to read the data
size_t string_read_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
  string_upload_info_s *upload_data = (string_upload_info_s *)userp;
  size_t max = size * nmemb;
  size_t bytes_remaining = upload_data->length - upload_data->read_pos;
  size_t bytes_to_copy = (bytes_remaining < max) ? bytes_remaining : max;

  if (bytes_to_copy > 0) {
    memcpy(ptr, upload_data->data + upload_data->read_pos, bytes_to_copy);
    upload_data->read_pos += bytes_to_copy;
  }

  return bytes_to_copy;
}

/* Upload a string to the S3 bucket.
 *
 * This assumes curl is initialized.
 */
bool uploader_upload_text(const char *data, const char *presigned_url) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (!curl) {
    fapp_logger_log(LOG_ERR, "Failed to initialize curl");
    return false;
  }

  string_upload_info_s upload_data = {
      .data = data, .length = strlen(data), .read_pos = 0};

  curl_easy_setopt(curl, CURLOPT_URL, presigned_url);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  curl_easy_setopt(curl, CURLOPT_READFUNCTION, string_read_callback);
  curl_easy_setopt(curl, CURLOPT_READDATA, &upload_data);
  curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)strlen(data));

  // Perform the upload
  res = curl_easy_perform(curl);

  // Check for errors
  if (res != CURLE_OK) {
    fapp_logger_log(LOG_ERR, "curl_easy_perform() to '%s' failed: %s",
                    presigned_url, curl_easy_strerror(res));
  }

  // Always cleanup
  curl_easy_cleanup(curl);

  return res == CURLE_OK;
}

/* Upload a file to the S3 bucket.
 *
 * This assumes curl is initialized.
 */
bool uploader_upload_file(const char *file_path, const char *presigned_url) {
  FILE *file;

  // Open the file to upload
  file = fopen(file_path, "r");
  if (!file) {
    fapp_logger_log(LOG_ERR, "Failed to open file %s", file_path);
    return false;
  }

  // Get the file size
  struct stat file_info;
  if (fstat(fileno(file), &file_info) != 0) {
    fapp_logger_log(LOG_ERR, "fstat error");
    return false;
  }
  long file_size = file_info.st_size;

  // Get file contents as a string
  char *file_contents = g_malloc(file_size + 1);
  if (file_contents == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to allocate memory for file contents");
    fclose(file);
    return false;
  }

  if (fread(file_contents, 1, file_size, file) != file_size) {
    fapp_logger_log(LOG_ERR, "Failed to read file contents");
    g_free(file_contents);
    fclose(file);
    return false;
  }

  file_contents[file_size] = '\0';
  bool res = uploader_upload_text(file_contents, presigned_url);
  g_free(file_contents);
  fclose(file);

  return res;
}