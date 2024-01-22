# These paths to docker images are set by the builder tool
ARG FAPP_IMAGE_BUILDER_BASE

# We use FixedIT Application base image - the version of the SDK
# is defined by the arguments to the build command.
FROM ${FAPP_IMAGE_BUILDER_BASE} as builder

WORKDIR /opt/app
COPY ./app .