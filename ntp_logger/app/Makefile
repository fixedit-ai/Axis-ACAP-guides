LIBFAPP_LIBRARY_PATH = fapp/

C_FILES = $(wildcard src/*.c) $(wildcard src/$(LIBFAPP_LIBRARY_PATH)/*.c)

PKGS = glib-2.0 gio-2.0

ifeq ($(PRESIGNED_URL_TIME_LOG),)
$(error PRESIGNED_URL_TIME_LOG is not set)
endif

ifeq ($(PRESIGNED_URL_NTP_STATUS),)
$(error PRESIGNED_URL_NTP_STATUS is not set)
endif

ifeq ($(PRESIGNED_URL_NTP_FAILED_STATUS),)
$(error PRESIGNED_URL_NTP_FAILED_STATUS is not set)
endif

CFLAGS += $(FIXEDIT_CFLAGS) -Iinclude \
          -DBIN_NAME="\"$(FIXEDIT_BIN_NAME)\"" \
          -DPRESIGNED_URL_TIME_LOG="\"$(PRESIGNED_URL_TIME_LOG)\"" \
          -DPRESIGNED_URL_NTP_STATUS="\"$(PRESIGNED_URL_NTP_STATUS)\"" \
          -DPRESIGNED_URL_NTP_FAILED_STATUS="\"$(PRESIGNED_URL_NTP_FAILED_STATUS)\"" \
          -DUPLOAD_INTERVAL_S=$(UPLOAD_INTERVAL_S)

LDFLAGS += -L./lib -Wl,-rpath,'$$ORIGIN/lib' $(FIXEDIT_LDFLAGS)

CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS)) \
          -lcurl ./lib/libssl.so ./lib/libcrypto.so

CFLAGS += -Wall -Werror -pedantic

C_OBJS = $(C_FILES:.c=.o)

all: $(FIXEDIT_BIN_NAME)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(FIXEDIT_BIN_NAME): $(C_OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
