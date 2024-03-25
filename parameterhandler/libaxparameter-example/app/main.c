// General header files
#include <signal.h>
#include <stdbool.h>
#include <syslog.h>
#include <unistd.h>

// App specific header files
#include "config.h"

#define RET_OK 0
#define RET_CONFIG_ERROR 1

volatile bool do_exit = false;

void sig_handler(int signum) {
  (void)signum;
  syslog(LOG_INFO, "Signal received, exiting...");
  do_exit = true;
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  const char *error = NULL;

  int return_code = RET_OK;

  // See manpages for options: https://linux.die.net/man/3/openlog
  openlog(APP_NAME, LOG_PID | LOG_CONS, LOG_USER);
  syslog(LOG_INFO, "Initializing application");

  signal(SIGINT, sig_handler);

  // Read the parameters from the configuration file
  if (!config_init()) {
    syslog(LOG_ERR, "Config initialization failed: %s", config_get_error());
    config_clear_error();
    return_code = RET_CONFIG_ERROR;
    goto error_out;
  }
  const char *cloud_url = config_get_cloud_url();
  int cloud_port = config_get_cloud_port();
  if ((error = config_get_error())) {
    syslog(LOG_ERR, "Failed to read cloud url or port from config: %s", error);
    config_clear_error();
    return_code = RET_CONFIG_ERROR;
    goto error_out;
  }
  syslog(LOG_INFO, "Reporting to server: %s:%d", cloud_url, cloud_port);

  syslog(LOG_INFO, "Set up finished.");
  while (!do_exit) {
    pause();
  }

error_out:
  config_destruct();
  closelog();
  return return_code;
}
