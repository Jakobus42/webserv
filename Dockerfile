FROM ubuntu:22.04

RUN apt-get update -y && apt-get upgrade -y && apt-get install -y \
    zsh \
    make \
    cppcheck \
    clang-format \
    gdb \
    git \
    valgrind \
    clang \
    clangd \
    vim \
    pkg-config \
    curl 

RUN mkdir -p /root/.ssh && chmod 700 /root/.ssh && \
    curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh | zsh && \
    git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions

WORKDIR /workspace
COPY . /workspace

RUN apt-get install -y bear && make fclean && bear -- make

CMD [ "zsh" ]