/**
 * Copyright (C) 2024 FixedIT Consulting AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#include <glib.h>
#include <stdint.h>

/* Run the chronyc tracking command to get the NTP status.
 *
 * Returns a GString that should be freed by the caller.
 */
GString *time_utils_get_ntp_status_info(GError **error);

/* Parse the output from the time_utils_get_ntp_status_info function
 * to get the status and difference.
 *
 * The 'status' should be freed by the caller.
 */
bool time_utils_parse_ntp_status(GString *ntp_info, char **status,
                                 double *difference);

uint64_t time_utils_get_system_epoch_time_us();

uint64_t time_utils_get_monotonic_time_us();