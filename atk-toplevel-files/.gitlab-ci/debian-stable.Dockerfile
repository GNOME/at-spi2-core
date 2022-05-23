FROM debian:stable

RUN apt-get update -qq && apt-get install --no-install-recommends -qq -y \
    ccache \
    dconf-gsettings-backend \
    g++ \
    gcc \
    gettext \
    git \
    gobject-introspection \
    itstool \
    libc6-dev \
    libgirepository1.0-dev \
    libglib2.0-dev \
    libwayland-dev \
    libx11-dev \
    libxml2-dev \
    libxrandr-dev \
    locales \
    ninja-build \
    pkg-config \
    python3 \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    shared-mime-info \
    wayland-protocols \
    xauth \
    xvfb \
 && rm -rf /usr/share/doc/* /usr/share/man/*

# Locale for our build
RUN locale-gen C.UTF-8 && /usr/sbin/update-locale LANG=C.UTF-8

RUN pip3 install meson==0.56.2

ARG HOST_USER_ID=5555
ENV HOST_USER_ID ${HOST_USER_ID}
RUN useradd -u $HOST_USER_ID -ms /bin/bash user

USER user
WORKDIR /home/user

ENV LANG=C.UTF-8 LANGUAGE=C.UTF-8 LC_ALL=C.UTF-8
