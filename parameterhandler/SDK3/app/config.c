/*

This file is a static wrapper around the AXParameter library used to store
parameters in a way that they can be changed from the camera web gui or VAPIX
and are safe through camera firmware updates etc.

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
#define PARAM_CLOUD_PORT "CloudPort"

static AXParameter *param_lib = NULL;
static GError *g_err = NULL;

static char *cloud_url = NULL;
static int cloud_port = -1;

static bool get_param(const char *name, char **buffer) {
  GError *new_err = NULL;
  bool ret = ax_parameter_get(param_lib, name, buffer, &new_err);
  if (!ret) {
    if (g_err) {
      g_error_free(g_err);
    }
    g_err = new_err;
    return false;
  }
  return true;
}

/* Public API functions */

bool config_init() {
  param_lib = ax_parameter_new(APP_NAME, &g_err);
  if (!param_lib) {
    return false;
  }
  return true;
}

void config_destruct() {
  if (cloud_url)
    g_free(cloud_url);
  if (param_lib)
    ax_parameter_free(param_lib);
  config_clear_error();
}

const char *config_get_error() {
  if (!g_err) {
    return NULL;
  }

  return g_err->message;
}

void config_clear_error() {
  if (g_err) {
    g_error_free(g_err);
  }
}

const char *config_get_cloud_url() {
  if (cloud_url) {
    return cloud_url;
  }
  // Lazy read of the actual parameter
  get_param(PARAM_CLOUD_URL, &cloud_url);
  return cloud_url;
}

int config_get_cloud_port() {
  if (cloud_port >= 0) {
    return cloud_port;
  }
  // Lazy read of the actual parameter
  char *cloud_port_str = NULL;
  get_param(PARAM_CLOUD_PORT, &cloud_port_str);
  if (cloud_port_str) {
    cloud_port = atoi(cloud_port_str);
    g_free(cloud_port_str);
  }

  return cloud_port;
}
