# libfapp source code is downloaded by the fappcli build tool
LIBFAPP_LIBRARY_PATH = fapp/

C_FILES = $(wildcard src/*.c) $(wildcard src/$(LIBFAPP_LIBRARY_PATH)/*.c)

PKGS = glib-2.0 gobject-2.0

CFLAGS += $(FAPP_CFLAGS) -Iinclude \
          -DBIN_NAME="\"$(FAPP_BIN_NAME)\""

LDFLAGS += -L./lib -Wl,-rpath,'$$ORIGIN/lib' $(FAPP_LDFLAGS)

CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS))
LDLIBS += -ljson-glib-1.0 -lcurl ./lib/libssl.so ./lib/libcrypto.so

CFLAGS += -Wall -Werror -pedantic

C_OBJS = $(C_FILES:.c=.o)

# For debuging memory errors or undefined behavior, the gcc sanitizers can be
# enabled. These does however make the code slow and keeping debug symbols will
# make the code easier to reverse engineer. Thus we only enable this when needed.
# The option flag is set by the fapp-manifest files.
ifeq ($(DEBUG_SANITIZE),true)
LDFLAGS += -fsanitize=address -static-libasan -fsanitize=undefined -static-libubsan
CFLAGS += -O0 -g -fno-omit-frame-pointer
endif

# Build targets
all: $(FAPP_BIN_NAME)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(FAPP_BIN_NAME): $(C_OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
