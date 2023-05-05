LIBFAPP_LIBRARY_PATH = fapp/

C_FILES = $(wildcard src/*.c) $(wildcard src/$(LIBFAPP_LIBRARY_PATH)/*.c)

PKGS = glib-2.0 gobject-2.0

CFLAGS += $(FAPP_CFLAGS) -Iinclude \
          -DAPP_NAME="\"$(FAPP_APP_NAME)\""

LDFLAGS += -L./lib -Wl,-rpath,'$$ORIGIN/lib' $(FAPP_LDFLAGS) \
           -ljson-glib-1.0 -lcurl ./lib/libssl.so ./lib/libcrypto.so

CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS))

CFLAGS += -Wall -Werror -pedantic

C_OBJS = $(C_FILES:.c=.o)

all: $(FAPP_BIN_NAME)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(FAPP_BIN_NAME): $(C_OBJS)
	$(CC) $^ $(LDLIBS) -o $@ $(LDFLAGS)
