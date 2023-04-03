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
	# Building and writing build output to `pwd`/$(PROG1)-build.log
	echo "ENVIRONMENT:" > $(PROG1)-build.log
	env >> $(PROG1)-build.log
	echo "\nBUILD LOG:" >> $(PROG1)-build.log
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@ >> $(PROG1)-build.log

clean:
	rm -f $(PROGS) *.o *.eap lib/* include/*
