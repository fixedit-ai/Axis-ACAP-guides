CAM_HOST ?= root@192.168.0.90
CAM_ARCH ?= armv7hf

FAPP_CFLAGS ?=
FAPP_MAKE_OPTS ?=

BUILDER_NAME=check_glibc-builder
APP_NAME=check_glibc
BINARY_NAME=check_glibc

MAKE_BUILD = 'source $$SDK_ENV && make ${FAPP_MAKE_OPTS}'
MAKE_PACKAGE = 'source $$SDK_ENV && acap-build .'

TIME = ${shell date +%Y%m%d%H%M%S}
GIT_HASH = ${shell git rev-parse --short HEAD}
GIT_STATE = ${shell test -z "`git status --porcelain`" || echo '-dirty'}
GIT_STR = $(GIT_HASH)$(GIT_STATE)
TAG = $(if $(GIT_STR),$(GIT_STR)-$(TIME),$(TIME))

ifeq (, $(shell which jq))
    $(error "jq is not installed, consider doing apt-get install jq")
endif
APP_VERSION := $(shell jq -r '.acapPackageConf.setup.version' app/manifest.json)
APP_FRIENDLY_NAME := $(shell jq -r '.acapPackageConf.setup.friendlyName' app/manifest.json)
APP_FILE_VERSION := $(subst .,_,$(APP_VERSION))
APP_FILE_NAME := $(subst $() $(),_,$(APP_FRIENDLY_NAME))

EAP_NAME := $(APP_FILE_NAME)_$(APP_FILE_VERSION)_$(CAM_ARCH).eap

all: install run

# ----------------------------------
# Targets affecting the build host
# ----------------------------------
build-builder:
	docker build . \
		--build-arg CAM_ARCH=${CAM_ARCH} \
		-t $(BUILDER_NAME):$(CAM_ARCH) -t $(BUILDER_NAME):$(TAG)-$(CAM_ARCH)

build: build-builder
	# Adding the -t flag makes sure that we get the color coding from gcc
	docker run --rm -t \
		-e FAPP_CFLAGS=${FAPP_CFLAGS} -e FAPP_MAKE_OPTS=${FAPP_MAKE_OPTS} \
		-v `pwd`/app:/opt/app \
		$(BUILDER_NAME):$(CAM_ARCH) bash -c $(MAKE_BUILD)

run-docker:
	docker run --rm -it \
		-v `pwd`/app:/opt/app \
		$(BUILDER_NAME):$(CAM_ARCH) bash

package: build-builder
	docker run --rm -t \
		-e FAPP_CFLAGS=${FAPP_CFLAGS} \
		-v `pwd`/app:/opt/app \
		$(BUILDER_NAME):$(CAM_ARCH) bash -c $(MAKE_PACKAGE)

cformat:
	clang-format -i app/src/*.[ch]

clean:
	make -C app clean

# ------------------------------
# Targets affecting the camera
# ------------------------------
read-log:
	ssh $(CAM_HOST) -t 'tail -f /var/log/*.log'

install: package
	# Install the acap as a package on the camera
	scp app/$(EAP_NAME) $(CAM_HOST):/tmp
	ssh $(CAM_HOST) "acap-install /tmp/$(EAP_NAME)"

install-bin: build
	# Copy the binary only, this is faster than installing the eap file but
	# can only be done when the eap file has been installed before
	scp app/$(BINARY_NAME) $(CAM_HOST):/usr/local/packages/$(APP_NAME)

run-bin: build
	# Copy and run only the binary from the /tmp dir, this works only
	# if the binary does not depend on any other files in the package
	scp app/$(BINARY_NAME) $(CAM_HOST):/tmp/$(BINARY_NAME)
	ssh -t $(CAM_HOST) /tmp/$(BINARY_NAME)

run: install-bin
	ssh -t $(CAM_HOST) /usr/local/packages/$(APP_NAME)/$(BINARY_NAME)
