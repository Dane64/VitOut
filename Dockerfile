ARG BUILD_PACKAGES="bzip2 ca-certificates ccache cmake curl jq ninja-build pkg-config unzip xz-utils"

# Refresh these probes with --no-cache-filter package-index,sdk-index.
FROM ubuntu:latest AS package-index
ARG BUILD_PACKAGES
RUN apt-get update --error-on=any \
    && apt-get install --simulate --no-install-recommends ${BUILD_PACKAGES} > /tmp/apt-plan \
    && sed -n '/^Inst /p' /tmp/apt-plan > /tooling-packages \
    && rm -rf /var/lib/apt/lists \
    && rm /tmp/apt-plan

FROM ubuntu:latest AS tooling
ARG BUILD_PACKAGES
COPY --from=package-index /tooling-packages /usr/local/share/vitout/tooling-packages
RUN apt-get update --error-on=any \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        ${BUILD_PACKAGES} \
    && rm -rf /var/lib/apt/lists

FROM tooling AS sdk-index
COPY scripts/resolve-sdk.sh /usr/local/bin/resolve-sdk
RUN bash /usr/local/bin/resolve-sdk /sdk-inputs

FROM tooling AS builder
ENV VITASDK=/opt/vitasdk
ENV PATH="${VITASDK}/bin:${PATH}"

COPY --from=sdk-index /sdk-inputs /usr/local/share/vitout/sdk-inputs
RUN bash /usr/local/share/vitout/sdk-inputs/bootstrap-vitasdk.sh \
    && VDPM_NONINTERACTIVE=1 vdpm install libvita2d \
    && dpkg-query -W > /usr/local/share/vitout/toolchain-versions \
    && vdpm list >> /usr/local/share/vitout/toolchain-versions \
    && sha256sum /usr/local/share/vitout/sdk-inputs/channel.json \
        >> /usr/local/share/vitout/toolchain-versions

ENV CCACHE_DIR=/ccache \
    CCACHE_BASEDIR=/src \
    CCACHE_COMPILERCHECK=content \
    CCACHE_MAXSIZE=256M

COPY --chmod=755 scripts/build.sh /usr/local/bin/build-vitout

WORKDIR /src
ENTRYPOINT ["/usr/local/bin/build-vitout"]
