#include <stdlib.h>

#include <curl/curl.h>
#include <fapp/logging.h> // Logging to both stdout and system log
#include <fapp/misc.h>    // For signal handling
#include <glib.h>
#include <json-glib/json-glib.h>

/* This struct defines the content of the todo API that we are getting data from
 */
typedef struct todo_item {
  int user_id;
  int id;
  GString *title;
  bool completed;
} todo_item_t;

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

/* This function parses the json string to extract the todo items.
 * The json is expected to be of this format:
 * {
 *     "userId": <INT>,
 *     "id": <INT>,
 *     "title": <STRING>,
 *     "completed": <BOOL>
 * }
 */
todo_item_t parse_item(GString *content) {
  todo_item_t item = {0};
  GError *error = NULL;
  JsonReader *reader = NULL;
  JsonParser *parser = json_parser_new();

  if (!json_parser_load_from_data(parser, content->str, content->len, &error)) {
    fapp_logger_log(LOG_ERR, "Failed to parse json: %s", error->message);
    goto end;
  }

  JsonNode *root = json_parser_get_root(parser); // Should not be free'd
  reader = json_reader_new(root);

  // Read the userID
  if (!json_reader_read_member(reader, "userId")) {
    fapp_logger_log(LOG_ERR, "JSON does not contain a userId tag");
    json_reader_end_element(reader);
    goto end;
  }
  item.user_id = json_reader_get_int_value(reader);
  json_reader_end_element(reader);

  // Read the todo item id
  if (!json_reader_read_member(reader, "id")) {
    fapp_logger_log(LOG_ERR, "JSON does not contain a id tag");
    json_reader_end_element(reader);
    goto end;
  }
  item.id = json_reader_get_int_value(reader);
  json_reader_end_element(reader);

  // Read the title
  if (!json_reader_read_member(reader, "title")) {
    fapp_logger_log(LOG_ERR, "JSON does not contain a title tag");
    json_reader_end_element(reader);
    goto end;
  }
  item.title = g_string_new(json_reader_get_string_value(reader));
  json_reader_end_element(reader);

  // Read the completed flag
  if (!json_reader_read_member(reader, "completed")) {
    fapp_logger_log(LOG_ERR, "JSON does not contain a completed tag");
    json_reader_end_element(reader);
    goto end;
  }
  item.completed = json_reader_get_boolean_value(reader);
  json_reader_end_element(reader);

end:
  g_object_unref(reader);
  g_object_unref(parser);
  if (error != NULL) {
    g_error_free(error);
  }
  return item;
}

/*
 * This function gets json data from a web API and prints it.
 */
static void get_and_print_json() {
  // Get the json data from the web API
  GString *json_string =
      web_get_json_data("https://jsonplaceholder.typicode.com/todos/1");
  if (json_string == NULL) {
    fapp_logger_log(LOG_ERR, "Failed to get json data");
    return;
  }

  // Parse the json data to a struct
  todo_item_t item = parse_item(json_string);

  // Print the data
  fapp_logger_log(LOG_INFO, "Parsed API data for todo-list:");
  fapp_logger_log(LOG_INFO, "* Item title: %s", item.title->str);
  fapp_logger_log(LOG_INFO, "  - Created by user ID: %d", item.user_id);
  fapp_logger_log(LOG_INFO, "  - Item ID: %d", item.id);
  fapp_logger_log(LOG_INFO, "  - Completed: %s", item.completed ? "yes" : "no");

  g_string_free(item.title, TRUE);
  g_string_free(json_string, TRUE);
}

int main(int argc, char **argv) {
  fapp_logger_init(BIN_NAME);

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
