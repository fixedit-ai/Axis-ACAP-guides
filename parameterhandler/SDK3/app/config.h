/*

This file is a static wrapper around the AXParameter library used to store
parameters in a way that they can be changed from the camera web gui or VAPIX and
are safe through camera firmware updates etc.

The wrapper is static and should thus only be initialized once.

This wrapper assumes that parameters are not changed during the operation of
the process, thus the parameters are cached in memory for rapid access. The
AXParameter library does support parameters changing at any time and can even
set a callback on change, this does however make the application considerably
more complicated than assuming a static parameter set after initializtion.  

Author: Daniel Falk, FixedIT Consulting AB, 2022

*/

#include <stdbool.h>

/* Call init function before using library and then finalize with destruct */
bool config_init();
void config_destruct();

/* If a function return an error, get the string representation of the
   error by calling the get error function. When done accessing the
   error message, free the buffer by calling the clear error function. */
const char *config_get_error();
void config_clear_error();

/* Functions to get string parameters, on error NULL is returned and the
   error message is set. */
const char *config_get_cloud_url();
