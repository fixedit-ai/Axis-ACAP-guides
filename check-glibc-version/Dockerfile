# Set a default SDK to make the ACAP application buildable
# without the FixedIT CLI tools.
ARG CAM_ARCH=armv7hf
ARG VERSION=3.5
ARG REPO=axisecp
ARG SDK=acap-sdk
ARG FIXEDIT_IMAGE_BUILDER_BASE=${REPO}/${SDK}:${VERSION}-${CAM_ARCH}

FROM ${FIXEDIT_IMAGE_BUILDER_BASE}

ENV SDK_ENV="/opt/axis/acapsdk/environment-setup*"

WORKDIR /opt/app
COPY ./app .