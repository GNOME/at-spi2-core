# Dockerfile to build container images for Gitlab Continuous Integration
#
# This starts with an openSUSE Tumbleweed image, and installs the dependencies
# for building and testing at-spi2-core.

FROM opensuse/tumbleweed:latest

RUN zypper refresh                              \
 && zypper install -y                           \
           clang                                \
           clang-tools                          \
           gcc                                  \
           dbus-1                               \
           dbus-1-devel                         \
           gettext                              \
           git                                  \
           glib2-devel                          \
           gobject-introspection-devel          \
           gsettings-desktop-schemas            \
           itstool                              \
           libasan6                             \
           libxml2-devel                        \
           libxkbcommon-devel                   \
           libXi-devel                          \
           libXtst-devel                        \
           lcov                                 \
           meson                                \
           ninja                                \
           python38                             \
           python38-gobject                     \
 && zypper clean --all
