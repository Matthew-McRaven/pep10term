# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.154.2/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="ubuntu-20.04"
ARG boost_version="1.76.0"
FROM registry.gitlab.com/pep10/libsim/emboost:v0.5.2

RUN rm -f /usr/bin/ld && ln -s /emsdk/llvm/git/build_main_64/bin/lld /usr/bin/ld