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
  if (!cloud_url) {
    syslog(LOG_ERR, "Failed to read cloud url from config: %s", config_get_error());
    config_clear_error();
    return_code = RET_CONFIG_ERROR;
    goto error_out;
  }
  syslog(LOG_INFO, "Reporting to server: %s", cloud_url);

  syslog(LOG_INFO, "Set up finished.");
  while (!do_exit) {
    sleep(999);
  }

error_out:
  config_destruct();
  closelog();
  return return_code;
}
