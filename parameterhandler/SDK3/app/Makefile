APP_NAME = ParameterhandlerTest
CFILES = main.c config.c
BINARIES = $(APP_NAME)

DEPENDS = "glib-2.0 axparameter"

CFLAGS += -DAPP_NAME="\"$(APP_NAME)\""
CFLAGS += -std=c99 -W -Wformat=2 -Wpointer-arith -Wbad-function-cast -Winline -Wdisabled-optimization -Wfloat-equal -Wall -Werror
CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(DEPENDS))
LIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(DEPENDS))

all: $(BINARIES)

$(APP_NAME): $(CFILES)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@
	$(STRIP) $@

clean:
	rm -f $(BINARIES) *.o core *.eap
