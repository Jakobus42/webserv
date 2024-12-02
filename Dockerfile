FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade && apt-get install -y \
    zsh \
    g++ \
    make \
    llvm \
    cppcheck \
    clang-format \
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
    wget \
    pip \
    doxygen 


RUN apt-get install -y locales && locale-gen en_US.UTF-8 && update-locale LANG=en_US.UTF-8
ENV LC_ALL en_US.UTF-8

RUN mkdir -p /root/.ssh && chmod 700 /root/.ssh && \
    curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh | zsh && \
    git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions

RUN pip install pre-commit

WORKDIR /workspace
COPY . /workspace

RUN apt-get install -y bear -- make re

RUN pre-commit install && pre-commit install --hook-type commit-msg

CMD [ "zsh" ]
