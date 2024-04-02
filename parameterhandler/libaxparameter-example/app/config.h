/*

This file is a static wrapper around the AXParameter library used to store
parameters in a way that they can be changed from the camera web gui or VAPIX
and are safe through camera firmware updates etc.

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

/* To check if a function returned an error, call the get error function. If no
   error has occured, NULL is returned. When done accessing the error message,
   clear the error and free the buffer by calling the clear error function. */
const char *config_get_error();
void config_clear_error();

/* Functions to get parameters, on error the error message is set. */
const char *config_get_cloud_url();
int config_get_cloud_port();