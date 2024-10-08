FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    zsh \
    g++ \
    make \
    llvm \
    cppcheck \
    clang-format \
    doxygen \
    gdb \
    git \
    valgrind \
    clang \
    clangd \
    vim \
    openssh-client \
    graphviz \
    ninja-build \
    gettext \
    libtool \
    libtool-bin \
    autoconf \
    automake \
    cmake \
    pkg-config \
    unzip \
    curl \
    pip

RUN mkdir -p /root/.ssh && chmod 700 /root/.ssh

RUN curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh | zsh && \
    git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions;

WORKDIR /workspace
COPY . /workspace

RUN pip install pre-commit && pre-commit install

CMD [ "zsh" ]