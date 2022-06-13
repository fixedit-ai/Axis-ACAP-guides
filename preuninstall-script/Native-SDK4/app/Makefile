# This Makefile is used in the Docker container.
# The container has already set up the cross-compilation environment,
# so there is normally no need to do that here.

PROG1	= preuninstall_test
OBJS1	= $(PROG1).c

PROGS	= $(PROG1)

CFLAGS-y  = -W -Wformat=2 -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Winline -Wdisabled-optimization -Wfloat-equal -Wall -Werror

all:	$(PROGS)

$(PROG1): $(OBJS1)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@
	$(STRIP) $@

clean:
	rm -f $(PROGS) *.o core *.eap
