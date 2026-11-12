FROM ubuntu:26.04

ARG USERNAME=vscode

ENV DEBIAN_FRONTEND=noninteractive

RUN if [ -f /etc/dpkg/dpkg.cfg.d/excludes ]; then \
        sed -i '\#^path-exclude=/usr/share/man/#d' /etc/dpkg/dpkg.cfg.d/excludes; \
    fi \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        bubblewrap \
        ca-certificates \
        catch2 \
        clang \
        clangd-21 \
        clang-format-21 \
        clang-tools \
        clang-tidy-21 \
        cmake \
        curl \
        g++ \
        gcc \
        gdb \
        git \
        hyperfine \
        lld \
        lldb \
        linux-tools-common \
        linux-tools-generic \
        less \
        make \
        man-db \
        manpages \
        manpages-dev \
        ninja-build \
        nodejs \
        npm \
        pkg-config \
        socat \
        sudo \
        time \
        valgrind \
        libbenchmark-dev \
    && if dpkg-divert --list /usr/bin/man | grep -q '/usr/bin/man'; then \
        if [ -e /usr/bin/man ] && [ -e /usr/bin/man.REAL ]; then \
            rm -f /usr/bin/man; \
        fi; \
        dpkg-divert --rename --remove /usr/bin/man; \
    fi \
    && mandb -c \
    && rm -rf /var/lib/apt/lists/*

RUN npm install -g @anthropic-ai/claude-code

RUN git config --system --add safe.directory /home/vscode/.ai-memories \
    && git config --system --add safe.directory /workspace/nasdaq_itch

COPY devcontainer-entrypoint /usr/local/bin/devcontainer-entrypoint
RUN chmod +x /usr/local/bin/devcontainer-entrypoint

RUN useradd --create-home --shell /bin/bash "${USERNAME}" \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > "/etc/sudoers.d/${USERNAME}" \
    && chmod 0440 "/etc/sudoers.d/${USERNAME}" \
    && mkdir -p "/home/${USERNAME}/.claude" \
    && chown "${USERNAME}:${USERNAME}" "/home/${USERNAME}/.claude"

WORKDIR /workspace/nasdaq_itch
