#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>

/* Log a message both to the system logger and to stdout */
void logmsg(int priority, const char *fmt, ...) {
  va_list args;

  /* Log to stdout */
  va_start(args, fmt);
  vprintf(fmt, args);
  printf("\n");
  va_end(args);

  /* Log to system logger */
  va_start(args, fmt);
  vsyslog(priority, fmt, args);
  va_end(args);
}

int main(void) {
  openlog(APP_NAME, LOG_PID | LOG_CONS, LOG_USER);

  logmsg(LOG_INFO, "Hello world!\n");
  return 0;
}
