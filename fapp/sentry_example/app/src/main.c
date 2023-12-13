#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include <fapp/logging.h> // Logging to both stdout and system log
#include <fapp/misc.h>    // For signal handling
#include <glib.h>
#include <sentry.h>
#include <vdo-error.h>
#include <vdo-stream.h>

/* Log a simple telemetry message to the Sentry.io server */
void sentry_log_message(const char *level, const char *message,
                        const char *description) {
  sentry_value_t event = sentry_value_new_event();
  sentry_value_set_by_key(event, "level", sentry_value_new_string(level));
  sentry_value_set_by_key(event, "message", sentry_value_new_string(message));

  if (description) {
    sentry_value_t message_info = sentry_value_new_object();
    sentry_value_set_by_key(message_info, "description",
                            sentry_value_new_string(description));

    sentry_value_t context = sentry_value_new_object();
    sentry_value_set_by_key(context, "Message Info", message_info);

    sentry_value_set_by_key(event, "contexts", context);
  }

  sentry_capture_event(event);
}

/* Log a message both to the camera log and the Sentry server */
void log_message_both(int level, const char *message, const char *description) {
  sentry_log_message(level == LOG_INFO ? "info" : "error", message,
                     description);
  if (description) {
    fapp_logger_log(LOG_INFO, "%s: %s", message, description);
  } else {
    fapp_logger_log(LOG_INFO, "%s", message);
  }
}

/* Send a telemetry message to sentry.io to log application update
 */
void sentry_log_app_updated_message(const char *old_version,
                                    const char *new_version) {
  sentry_value_t event = sentry_value_new_event();
  sentry_value_set_by_key(event, "level", sentry_value_new_string("info"));
  sentry_value_set_by_key(event, "message",
                          sentry_value_new_string("Application updated"));

  sentry_value_t update_info = sentry_value_new_object();
  sentry_value_set_by_key(update_info, "old_version",
                          sentry_value_new_string(old_version));
  sentry_value_set_by_key(update_info, "new_version",
                          sentry_value_new_string(new_version));

  sentry_value_t context = sentry_value_new_object();
  sentry_value_set_by_key(context, "Update Info", update_info);

  sentry_value_set_by_key(event, "contexts", context);
  sentry_capture_event(event);
}

/* Log a start-up message to Sentry so that we can track how frequently
 * the application is restarting. If the application is started for the
 * first time since being installed, also log an installed message.
 */
void log_startup() {
  log_message_both(LOG_INFO, "Application started", NULL);

  // Check if the application has just now been installed. We do this
  // by writing to a file in the localdata directory that we can look at,
  // the file contains the version of the application, so we can
  // also see if the application has been updated or if it was a
  // fresh install.
  GError *error = NULL;
  const char *file_path = APP_DIR "/localdata/app_envoy";
  // Test if the file exists - if not, this is the first time the
  // application is started after being freshly installed.
  if (!g_file_test(file_path, G_FILE_TEST_EXISTS)) {
    log_message_both(LOG_INFO, "Application installed", NULL);
    if (!g_file_set_contents(file_path, APP_VERSION, -1, &error)) {
      log_message_both(LOG_ERR, "Failed to write app_envoy file",
                       error ? error->message : NULL);
    }
  } else {
    // File exists - if app version is same, do nothing more, otherwise
    // log an app updated message and update the file contents.
    char *old_version = NULL;
    if (g_file_get_contents(file_path, &old_version, NULL, &error)) {
      g_strstrip(old_version);
      if (strcmp(old_version, APP_VERSION) != 0) {
        sentry_log_app_updated_message(old_version, APP_VERSION);
        fapp_logger_log(LOG_INFO, "Application updated from %s to %s",
                        old_version, APP_VERSION);
        if (!g_file_set_contents(file_path, APP_VERSION, -1, &error)) {
          log_message_both(LOG_ERR, "Failed to write app_envoy file",
                           error ? error->message : NULL);
        }
      }
      g_free(old_version);
    } else {
      log_message_both(LOG_ERR, "Failed to read app_envoy file",
                       error ? error->message : NULL);
    }
  }
}

/* Start a Sentry profiling context.
 * The profiling context is used to measure how much time different spans
 * of a code section takes to execute.
 * Read more here: https://blog.sentry.io/profiling-101-what-is-profiling/
 */
sentry_transaction_t *sentry_start_profiling(const char *name) {
  sentry_transaction_context_t *tx_ctx =
      sentry_transaction_context_new("Profile", name);
  sentry_transaction_t *tx =
      sentry_transaction_start(tx_ctx, sentry_value_new_null());
  return tx;
}

/* Start a new span in the profiling context */
sentry_span_t *sentry_start_subprofiling(sentry_transaction_t *tx,
                                         const char *name) {
  sentry_span_t *span = sentry_transaction_start_child(tx, "Profile", name);
  return span;
}

void sentry_stop_subprofiling(sentry_span_t *span) { sentry_span_finish(span); }

void sentry_stop_profiling(sentry_transaction_t *tx) {
  sentry_transaction_finish(tx);
}

/* Get the name of the service account that is running the application.
 */
static char *get_username() {
  struct passwd *pw = getpwuid(getuid());
  if (pw) {
    return pw->pw_name;
  }
  return NULL;
}

/* Get the group name of the service account that is running the application.
 */
static char *get_groupname() {
  struct group *gr = getgrgid(getgid());
  if (gr) {
    return gr->gr_name;
  }
  return NULL;
}

/* Initialize logging to Sentry.io.
 * The DSN is the unique identifier for the server to log to. Other options
 * affects how the telemetry is sent or what metadata is attached to it.
 */
static void fapp_sentry_init() {
  sentry_options_t *options = sentry_options_new();
  sentry_options_set_dsn(options, SENTRY_DSN);
  sentry_options_set_environment(options, SENTRY_ENV);
  sentry_options_set_database_path(options, APP_DIR "/localdata/sentry");
  sentry_options_set_release(options, APP_VERSION);
  sentry_options_set_debug(options, SENTRY_DEBUG);

  // Set the sampling rate for performance metrics. Since we measure
  // the analytics of every frame we will get ~25 samples per second.
  // We should only send a fraction of these to Sentry.
  sentry_options_set_traces_sample_rate(options, 1. / 25 / 60 / 2);

  sentry_init(options);

  // Add tags to the current scope that can be used to filter or search
  // for events in Sentry.
  sentry_set_tag("camera_name", g_get_host_name());
  sentry_set_tag("app_username", get_username());
  sentry_set_tag("app_groupname", get_groupname());
}

static void fapp_sentry_cleanup() { sentry_close(); }

/* Calculate the ratio of pixels which are at least 50% of brightness.
 * This is a dummy function to simulate some kind of image analytics.
 * The image can be a gray scale image or a planar/semi-planar YUV image
 * like NV12 in which only the gray plane is used.
 */
static float calc_bright_values(gpointer data, size_t width, size_t height) {
  size_t count = 0;
  for (size_t i = 0; i < width * height; i++) {
    if (((uint8_t *)data)[i] > 128) {
      count++;
    }
  }
  return (float)count / (width * height);
}

static void run_analytics() {
  size_t width = 1920;
  size_t height = 1080;

  // Setup the video stream
  GError *error = NULL;
  VdoBuffer *buffer = NULL;
  VdoMap *vdo_settings = vdo_map_new();
  vdo_map_set_uint32(vdo_settings, "format", VDO_FORMAT_YUV);
  vdo_map_set_string(vdo_settings, "subformat", "NV12");
  vdo_map_set_uint32(vdo_settings, "width", width);
  vdo_map_set_uint32(vdo_settings, "height", height);
  vdo_map_set_uint32(vdo_settings, "buffer.strategy",
                     VDO_BUFFER_STRATEGY_INFINITE);
  VdoStream *stream = vdo_stream_new(vdo_settings, NULL, &error);

  if (!stream || !vdo_stream_start(stream, &error)) {
    log_message_both(LOG_ERR, "Failed to start video stream",
                     error ? error->message : NULL);
    goto cleanup;
  }

  // Run some dummy analytics in the video frames
  sentry_transaction_t *profiling;
  sentry_span_t *subprofiling;
  size_t i = 0;
  while (!fapp_do_stop) {
    profiling = sentry_start_profiling("main_loop");
    // Get a frame
    subprofiling = sentry_start_subprofiling(profiling, "get_frame");
    buffer = vdo_stream_get_buffer(stream, &error);
    if (!buffer) {
      if (error->code == VDO_ERROR_NO_DATA) {
        g_clear_error(&error);
        fapp_logger_log(LOG_INFO, "No data");
        continue;
      }
      log_message_both(LOG_ERR, "Failed to get frame",
                       error ? error->message : NULL);
      goto cleanup;
    }
    sentry_stop_subprofiling(subprofiling);

    // Count pixels above 50% brightness
    subprofiling = sentry_start_subprofiling(profiling, "analytics");
    float brightness =
        calc_bright_values(vdo_buffer_get_data(buffer), width, height);
    sentry_stop_subprofiling(subprofiling);

    if (i++ % 100 == 0) {
      fapp_logger_log(LOG_INFO, "Brightness: %f", brightness);
    }

    // Free the image frame
    vdo_stream_buffer_unref(stream, &buffer, NULL);
    sentry_stop_profiling(profiling);
  }

cleanup:
  if (buffer) { // Needed in case of error goto
    g_object_unref(buffer);
  }
  if (error) {
    g_error_free(error);
  }
  if (stream) {
    vdo_stream_stop(stream);
    g_object_unref(stream);
  }
  g_object_unref(vdo_settings);
}

int main(int argc, char **argv) {
  fapp_logger_init(APP_NAME);

  // Setup a signal handler for gracefull shutdown. Note that Sentry will
  // override some of the handlers to capture stack traces. If we need to
  // do some cleanup on errors we can add an on_crash callback to Sentry.
  fapp_signal_handler_init();

  // Init remote logging/telemetry to sentry
  fapp_sentry_init();
  log_startup(); // Log application startup message

  // Do useful work
  while (true) {
    run_analytics();
    if (fapp_do_stop) {
      break;
    }
    sleep(5); // Wait a while before retrying after error..
  }

  // Clean up
  fapp_sentry_cleanup();

  fapp_logger_log(LOG_INFO, "App exited.");
  return EXIT_SUCCESS;
}
