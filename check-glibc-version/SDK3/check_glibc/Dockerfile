ARG CAM_ARCH=armv7hf
ARG VERSION=3.4.2
ARG REPO=axisecp
ARG SDK=acap-sdk

FROM ${REPO}/${SDK}:${VERSION}-${CAM_ARCH}

RUN apt-get update && apt-get install -y \
      openssh-client

ENV SDK_ENV="/opt/axis/acapsdk/environment-setup*"

WORKDIR /opt/app
COPY ./app .
