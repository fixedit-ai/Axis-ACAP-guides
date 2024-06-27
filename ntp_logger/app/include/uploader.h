/**
 * Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#pragma once
#include <stdbool.h>

bool uploader_check_if_time_to_upload();
bool uploader_upload_file(const char *file_path, const char *presigned_url);
bool uploader_upload_text(const char *text, const char *presigned_url);