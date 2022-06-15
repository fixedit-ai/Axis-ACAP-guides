/*

This file is a static wrapper around the AXParameter library used to store
parameters in a way that they can be changed from the camera web gui or VAPIX and
are safe through camera firmware updates etc.

The wrapper is static and should thus only be initialized once.

This wrapper assumes that parameters are not changed during the operation of
the process, thus the parameters are cached in memory for rapid access. The
AXParameter library does support parameters changing at any time and can even
set a callback on change, this does however make the application considerably
more complicated than assuming a static parameter set after initializtion.

Author: Daniel Falk, FixedIT Consulting AB, 2022

*/

// App specific header files
#include "config.h"

// General header files
#include <glib.h>

// ACAP SDK specific header files
#include <axsdk/axparameter.h>

#define PARAM_CLOUD_URL "CloudUrl"

static AXParameter *param_lib = NULL;
static GError *g_err = NULL;

static gchar *cloud_url = NULL;

/* Public API functions */

bool config_init() {
  param_lib = ax_parameter_new(APP_NAME, &g_err);
  if (!param_lib) {
    return false;
  }
  return true;
}

void config_destruct() {
  if (cloud_url) g_free(cloud_url);
  if (param_lib) ax_parameter_free(param_lib);
  config_clear_error();
}

const char *config_get_error() {
  if (!g_err) {
    return "";
  }

  return g_err->message;
}

void config_clear_error() {
  if (g_err) {
    g_error_free(g_err);
  }
}

const char *config_get_cloud_url() {
  if (!cloud_url) {
    // Lazy read of the actual parameter
    if (!ax_parameter_get(param_lib, PARAM_CLOUD_URL, &cloud_url, &g_err)) {
      return NULL;
    }
  }

  return (char *)cloud_url;
}
