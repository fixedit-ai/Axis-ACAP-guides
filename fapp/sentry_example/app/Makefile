# libfapp source code is downloaded by the fappcli build tool
LIBFAPP_LIBRARY_PATH = fapp/

# These variables should be set by the build tool either as CLI options
# or as data in the fapp-manifest files.
ifndef SENTRY_DSN
$(error SENTRY_DSN is not set)
endif
ifndef SENTRY_ENV
$(error SENTRY_ENV is not set)
endif

# Flags are already set by the SDK, here we can add any of our own additions
# and specify which C-files and header include directories to use.

C_FILES = $(wildcard src/*.c) $(wildcard src/$(LIBFAPP_LIBRARY_PATH)/*.c)

PKGS = glib-2.0 gobject-2.0

CFLAGS += $(FAPP_CFLAGS) -Iinclude \
          -DAPP_NAME="\"$(FAPP_APP_NAME)\"" \
          -DAPP_DIR="\"/usr/local/packages/$(FAPP_APP_NAME)/\"" \
          -DAPP_VERSION="\"$(FAPP_APP_VERSION)\"" \
          -DSENTRY_DSN="\"$(SENTRY_DSN)\"" \
          -DSENTRY_ENV="\"$(SENTRY_ENV)\"" \
          -DSENTRY_DEBUG=$(if $(filter true,${VERBOSE_OUTPUT}),1,0)

LDFLAGS += -L./lib -Wl,-rpath,'$$ORIGIN/lib' $(FAPP_LDFLAGS) \
           -lsentry -lvdostream

CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS))

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
	$(CC) $^ $(LDLIBS) -o $@ $(LDFLAGS)
