subdir('adaptors')

atk_bridge_sources = [
  introspection_generated,
  'accessible-leasing.c',
  'accessible-cache.c',
  'accessible-register.c',
  'accessible-stateset.c',
  'bridge.c',
  'object.c',
  'event.c',
  'spi-dbus.c',
]

install_headers([ 'atk-bridge.h' ], subdir: join_paths('at-spi2-atk', '2.0'))

libatk_bridge = library('atk-bridge-2.0', atk_bridge_sources,
                        include_directories: root_inc,
                        dependencies: [
                          libatk_bridge_adaptors_dep,
                          libdroute_dep,
                          libdbus_dep,
                          gio_dep,
                          gmodule_dep,
                          gobject_dep,
                          libatk_dep,
                          atspi_dep,
                        ],
                        version: atk_bridge_libversion,
                        soversion: atk_bridge_soversion,
                        install: true)

libatk_bridge_dep = declare_dependency(link_with: libatk_bridge,
                                       include_directories: [
                                         root_inc,
                                         include_directories('.')
                                       ])

if get_option('gtk2_atk_adaptor')
  atk_bridge_module = shared_module('atk-bridge', 'gtk-2.0/module.c',
                                    include_directories: root_inc,
                                    dependencies: [
                                      libatk_bridge_dep,
                                      libdbus_dep,
                                      libatk_dep,
                                      atspi_dep,
                                      gmodule_dep,
                                    ],
                                    install: true,
                                    install_dir: join_paths(get_option('libdir'), 'gtk-2.0', 'modules'))
endif

pkgconfig = import('pkgconfig')
pkgconfig.generate(
  name: 'atk-bridge-2.0',
  description: 'ATK/D-Bus Bridge',
  version: atk_bridge_version,
  libraries: libatk_bridge,
  requires_private: ['gobject-2.0', 'atspi-2'],
  filebase: 'atk-bridge-2.0',
  subdirs: 'at-spi2-atk/2.0',
  install_dir: join_paths(get_option('libdir'), 'pkgconfig'),
)
