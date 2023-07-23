#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#ifdef __GLIBC__
#include <gnu/libc-version.h>
#endif
#include <glib.h>

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

  logmsg(LOG_INFO, "GLIB compile-time version:\t%d.%d.%d", GLIB_MAJOR_VERSION,
         GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
  logmsg(LOG_INFO, "GLIB runtime version:     \t%d.%d.%d", glib_major_version,
         glib_minor_version, glib_micro_version);

#ifdef __GLIBC__
  logmsg(LOG_INFO, "GNU libc compile-time version:\t%u.%u", __GLIBC__,
         __GLIBC_MINOR__);
  logmsg(LOG_INFO, "GNU libc runtime version:     \t%s",
         gnu_get_libc_version());
  return 0;
#else
  logmsg(LOG_INFO, "Not the GNU C Library");
  return 1;
#endif
}
