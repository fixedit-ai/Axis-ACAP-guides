ARG CAM_ARCH
ARG VERSION=1.5
ARG REPO=axisecp
ARG SDK=acap-native-sdk

FROM ${REPO}/${SDK}:${VERSION}-${CAM_ARCH} as builder
ENV SDK_ENV="/opt/axis/acapsdk/environment-setup*"

WORKDIR /opt/app
COPY ./app .

# Image with only the application binary built
FROM builder as app-binaries
ARG FAPP_BIN_NAME
ARG FAPP_APP_NAME
ARG FAPP_MAKE_OPTS
ARG FAPP_CFLAGS
ARG FAPP_LDFLAGS
RUN . $SDK_ENV && make $FAPP_MAKE_OPTS

# Image with the application .eap package built
FROM builder as app-package
ARG FAPP_BIN_NAME
ARG FAPP_APP_NAME
ARG FAPP_CFLAGS
ARG FAPP_LDFLAGS
RUN . $SDK_ENV && acap-build .
