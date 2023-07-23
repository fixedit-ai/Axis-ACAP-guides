PROG1	= check_glibc
OBJS1	= src/$(PROG1).c
PROGS	= $(PROG1)

PKGS = glib-2.0

CFLAGS  += $(FAPP_CFLAGS) -Iinclude

LDFLAGS += -L./lib -Wl,-rpath,'$$ORIGIN/lib'

CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS))

CFLAGS += -Wall -Werror -pedantic

all: $(PROGS)

$(PROG1): $(OBJS1)
	# Write build env info to the file `$(PROG1)-build.env`
	env >> $(PROG1)-build.env

	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -f $(PROGS) *.o *.eap *.eap.old *_LICENSE.txt $(PROG1)-build.env package.conf package.conf.orig param.conf
