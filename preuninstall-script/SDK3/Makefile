CAM_NAME ?= 192.168.0.90
CAM_ARCH ?= armv7hf
APP_NAME ?= preuninstall_test

BUILD_DIR ?= build

.PHONY: build
build: ${BUILD_DIR}_${CAM_ARCH}

${BUILD_DIR}_${CAM_ARCH}: app
	# Cross compile the acap to a .eap package
	docker build . --build-arg ARCH=${CAM_ARCH} --tag ${APP_NAME}:${CAM_ARCH}
	rm -rf ${BUILD_DIR}_${CAM_ARCH}
	APP_IMG=`docker create ${APP_NAME}:${CAM_ARCH}`; docker cp $$APP_IMG:/opt/app ${BUILD_DIR}_${CAM_ARCH}; docker rm $$APP_IMG

install-eap: ${BUILD_DIR}_${CAM_ARCH}
	# Install the acap as a package
	scp ${BUILD_DIR}_${CAM_ARCH}/*.eap $(CAM_NAME):/tmp
	ssh $(CAM_NAME) 'acap-install /tmp/*${CAM_ARCH}.eap'

cformat:
	clang-format -i app/*.[ch]

read-log:
	ssh $(CAM_NAME) -t 'tail -f /var/log/*.log'
