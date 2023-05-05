# These paths to docker images are set by the builder tool
ARG FAPP_IMAGE_BUILDER_BASE
ARG FAPP_IMAGE_OPENSSL
ARG FAPP_IMAGE_CURL
ARG FAPP_IMAGE_JSON_GLIB

# Load the prebuilt libraries for curl so that we can make
# web requests from the app. Openssl is needed if we want to
# use https.
FROM ${FAPP_IMAGE_CURL} as library-curl
FROM ${FAPP_IMAGE_OPENSSL} as library-openssl
FROM ${FAPP_IMAGE_JSON_GLIB} as library-json-glib

# We use FixedIT Application base image - the version of the SDK
# is defined by the arguments to the build command.
FROM ${FAPP_IMAGE_BUILDER_BASE} as builder

# Copy the prebuilt libraries from the library images
COPY --from=library-openssl /library /opt/app
COPY --from=library-curl /library /opt/app
COPY --from=library-json-glib /library /opt/app

# Copy app source code
WORKDIR /opt/app
COPY ./app .

# Append library license files to the app license file
# This will be visible in the application tab in the cameras UI.
RUN cat license/* >> LICENSE
