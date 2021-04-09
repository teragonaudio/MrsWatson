FROM ubuntu:18.04
RUN apt-get update && apt-get install -y gcc g++ gdb gdbserver make cmake g++-multilib libc6-dev libc6-dev-i386 libx11-dev
RUN echo "set auto-load safe-path /" >> /root/.gdbinit