FROM ubuntu:16.04

RUN mkdir -p /home/developer
RUN apt-get update && apt-get install -yq \
    sudo \
    cowsay \
    git \
    tmux \
    wget \
    zsh \
    vim \
    fonts-powerline \
    gitk \
    meld \
    tree \
    mc \
    g++ \
    automake \
    # cmake \
    cmake-curses-gui \
    xz-utils

WORKDIR /opt
# ZSH config
RUN git clone https://github.com/robbyrussell/oh-my-zsh && \
    cp oh-my-zsh/templates/zshrc.zsh-template /home/developer/.zshrc && \
    cp -r oh-my-zsh /home/developer/.oh-my-zsh && \
    sed  "s/robbyrussell/gentoo/" -i /home/developer/.zshrc && \
    echo "PROMPT=\$(echo \$PROMPT | sed 's/%m/%f\$IMAGE_NAME/g')" >> /home/developer/.zshrc && \
    echo "RPROMPT=''" >> /home/developer/.zshrc

# Tmux config
RUN git clone https://github.com/gpakosz/.tmux.git && \
    echo "set-option -g default-shell /bin/zsh" >> .tmux/.tmux.conf
COPY onStartup/tmux_setup.sh /opt/startup/

# VS Code
RUN wget https://go.microsoft.com/fwlink/?LinkID=760868 --no-check-certificate -O /opt/code.deb && \
    sudo dpkg -i code.deb; sudo apt-get install -fy

# Sublime
ARG SUBLIME_BUILD="${SUBLIME_BUILD:-3143}"
RUN wget https://download.sublimetext.com/sublime-text_build-"${SUBLIME_BUILD}"_amd64.deb --no-check-certificate -O subl.deb && \
    dpkg -i -R subl.deb

# Cmake
#######################################################################
#
#       Currently the cmake 2.8 is enough for building SDL.
#       Uncomment following lines in order to use CMake 3.11.0
#
#######################################################################
# ARG CMAKE_PREFIX="/opt/cmake-3.11.0/bin"
# RUN wget --no-check-certificate https://cmake.org/files/v3.11/cmake-3.11.0.tar.gz && \
#     tar -xzvf cmake-3.11.0.tar.gz && \
#     cd cmake-3.11.0 && \
#     ./bootstrap --prefix=$CMAKE_PREFIX && \
#     make && \
#     make install


COPY onStartup/set_env.sh /opt/startup/
COPY onStartup/entrypoint.sh /usr/bin/

WORKDIR /home/developer
ENTRYPOINT ["/bin/bash", "-e", "/usr/bin/entrypoint.sh"]
CMD ["/bin/zsh"]
