ARG CAM_ARCH=armv7hf
ARG VERSION=3.5
ARG REPO=axisecp
ARG SDK=acap-sdk

FROM ${REPO}/${SDK}:${VERSION}-${CAM_ARCH}

ENV SDK_ENV="/opt/axis/acapsdk/environment-setup*"

WORKDIR /opt/app
COPY ./app .