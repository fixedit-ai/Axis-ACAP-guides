ARG FIXEDIT_IMAGE_BUILDER_BASE
ARG FIXEDIT_IMAGE_OPENSSL
ARG FIXEDIT_IMAGE_CURL

# We use FixedIT Application base image - the version of the SDK
# is defined by the arguments to the build command.
FROM ${FIXEDIT_IMAGE_CURL} AS library-curl
FROM ${FIXEDIT_IMAGE_OPENSSL} AS library-openssl
FROM ${FIXEDIT_IMAGE_BUILDER_BASE} AS builder

# Copy the prebuilt libraries from the library images
COPY --from=library-openssl /library /opt/app
COPY --from=library-curl /library /opt/app

# Copy app source code
WORKDIR /opt/app
COPY ./app .

# Append library license files to the app license file
# This will be visible in the application tab in the cameras UI.
RUN if (test -d license); then cat license/* >> LICENSE; fi
