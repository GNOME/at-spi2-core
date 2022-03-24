FROM fedora:latest

RUN dnf -y install \
    at-spi2-atk-devel \
    ccache \
    dbus-x11 \
    gcc \
    gcc-c++ \
    gettext \
    gettext-devel \
    git \
    glib2-devel \
    gobject-introspection-devel \
    gtk-doc \
    iso-codes \
    itstool \
    make \
    meson \
    ninja-build \
    python3 \
    python3-pip \
    python3-wheel \
    redhat-rpm-config \
    wayland-devel \
    wayland-protocols-devel \
    xorg-x11-server-Xvfb \
    && dnf clean all

RUN pip3 install meson==0.56.2

ARG HOST_USER_ID=5555
ENV HOST_USER_ID ${HOST_USER_ID}
RUN useradd -u $HOST_USER_ID -ms /bin/bash user

USER user
WORKDIR /home/user

ENV LANG C.utf8
