ARG CAM_ARCH=armv7hf
ARG VERSION=1.8
ARG REPO=axisecp
ARG SDK=acap-native-sdk

FROM ${REPO}/${SDK}:${VERSION}-${CAM_ARCH}

ENV SDK_ENV="/opt/axis/acapsdk/environment-setup*"

WORKDIR /opt/app
COPY ./app .
