subdir('adaptors')

atk_bridge_sources = [
  'accessible-leasing.c',
  'accessible-cache.c',
  'accessible-register.c',
  'accessible-stateset.c',
  'introspection.c',
  'bridge.c',
  'object.c',
  'event.c',
  'spi-dbus.c',
]

install_headers([ 'atk-bridge.h' ], subdir: join_paths(meson.project_name(), '2.0'))

libatk_bridge = shared_library('atk-bridge-2.0', atk_bridge_sources,
                               include_directories: root_inc,
                               dependencies: [
                                 libatk_bridge_adaptors_dep,
                                 libdroute_dep,
                                 libdbus_dep,
                                 gmodule_dep,
                                 atk_dep,
                                 atspi_dep,
                               ],
                               c_args: p2p_cflags,
                               version: atk_bridge_libversion,
                               soversion: atk_bridge_soversion,
                               install: true)

libatk_bridge_dep = declare_dependency(link_with: libatk_bridge,
                                       include_directories: [
                                         root_inc,
                                         include_directories('.')
                                       ])

atk_bridge_module = shared_module('atk-bridge', 'gtk-2.0/module.c',
                                  include_directories: root_inc,
                                  dependencies: [
                                    libatk_bridge_dep,
                                    libdbus_dep,
                                    atk_dep,
                                    atspi_dep,
                                    gmodule_dep,
                                  ],
                                  c_args: p2p_cflags,
                                  install: true,
                                  install_dir: join_paths(get_option('libdir'), 'gtk-2.0', 'modules'))
