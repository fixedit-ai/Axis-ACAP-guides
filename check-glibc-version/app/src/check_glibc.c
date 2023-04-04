#include <stdio.h>
#ifdef __GLIBC__
#include <gnu/libc-version.h>
#endif
#include <glib.h>

int main(void) {
  printf("GLIB compile-time version:\t%d.%d.%d\n", GLIB_MAJOR_VERSION,
         GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
  printf("GLIB runtime version:     \t%d.%d.%d\n", glib_major_version,
         glib_minor_version, glib_micro_version);

#ifdef __GLIBC__
  printf("GNU libc compile-time version:\t%u.%u\n", __GLIBC__, __GLIBC_MINOR__);
  printf("GNU libc runtime version:     \t%s\n", gnu_get_libc_version());
  return 0;
#else
  puts("Not the GNU C Library");
  return 1;
#endif
}
