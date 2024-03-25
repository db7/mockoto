FROM ubuntu:22.04

RUN apt update && apt install -y cmake make clang libclang-dev git xxd

RUN GIT_SSL_NO_VERIFY=true git clone https://github.com/db7/mockoto

RUN cd mockoto && \
	cmake -S . -Bbuild && \
	cmake --build build && \
	cmake --install build


