#include <stdlib.h>

#include <curl/curl.h>
#include <fapp/logging.h> // Logging to both stdout and system log
#include <fapp/misc.h>    // For signal handling
#include <glib.h>

// Use the cameras certificate authority to validate certs
#define SSL_CA_PATH "/etc/ssl/certs"


/*
 * This function is called by libcurl when it has data to write and
 * appends the data to a GString buffer.
 */
static size_t write_curl_response_to_gstring(void *contents, size_t size,
                                             size_t nmemb, void *data) {
  GString *json_string = (GString *)data;

  size_t realsize = size * nmemb;
  g_string_append_len(json_string, (char *)contents, realsize);

  return realsize;
}


/*
 * This function uses libcurl to get json data from a web API.
 * The data is returned as a GString that must be freed by the caller.
 * Returns NULL on error.
 */
static GString *web_get_json_data(const char *url) {
  GString *ret = NULL;

  CURL *curl;
  CURLcode curl_retval;

  GString *json_string = g_string_new(NULL);

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_curl_response_to_gstring);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)json_string);
  curl_easy_setopt(curl, CURLOPT_CAPATH, SSL_CA_PATH);

  curl_retval = curl_easy_perform(curl);
  if (curl_retval != CURLE_OK) {
    fapp_logger_log(LOG_ERR, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(curl_retval));
    g_string_free(json_string, TRUE);
    goto end;
  }

  ret = json_string;

end:
  curl_easy_cleanup(curl);

  return ret;
}


/*
 * This function gets json data from a web API and prints it.
 */
static void get_and_print_json() {
  GString *json_string =
      web_get_json_data("https://jsonplaceholder.typicode.com/todos/1");
  if (json_string == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to get json data");
    return;
  }
  fapp_logger_log(LOG_INFO, "JSON data: %s", json_string->str);
  g_string_free(json_string, TRUE);
}


int main(int argc, char **argv) {
  fapp_logger_init(APP_NAME);

  // Setup a signal handler so that we can perform clean-up and
  // return of peripherals when receiving a signal or when an
  // error condition such as segfaults is detected.
  fapp_signal_handler_init();

  // Initialize cURL that will be used for HTTPS requests to get json data from
  // web APIs
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // Write hello world!
  fapp_logger_log(LOG_INFO, "Hello world!");

  // Get json data from a web API and print it
  get_and_print_json();

  // Clean up
  curl_global_cleanup();

  fapp_logger_log(LOG_INFO, "App exited.");
  return EXIT_SUCCESS;
}
