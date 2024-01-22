#include <stdlib.h>

#include <fapp/logging.h> // Logging to both stdout and system log
#include <fapp/misc.h>    // For signal handling

int main(int argc, char **argv) {
  fapp_logger_init(BIN_NAME);

  // Setup a signal handler so that we can perform clean-up and
  // return of peripherals when receiving a signal or when an
  // error condition such as segfaults is detected.
  fapp_signal_handler_init();

  // Create a glib event loop and register a callback to stop it
  // whenever a signal is recevied
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  fapp_signal_handler_register_gmainloop(loop);

  // Write hello world!
  fapp_logger_log(LOG_INFO, "I am %s with version %s!", APP_NAME, APP_VERSION);

  // Run main loop until receiving a signal
  while (!fapp_do_stop) {
    g_main_loop_run(loop);
  }

  fapp_logger_log(LOG_INFO, "App exited.");

  return EXIT_SUCCESS;
}
