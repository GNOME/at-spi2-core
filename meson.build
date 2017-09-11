project('at-spi2-atk', 'c',
        version: '2.26.0',
        license: 'LGPLv2.1+',
        default_options: [
          'buildtype=debugoptimized',
          'warning_level=1',
          'c_std=c99',
        ],
        meson_version : '>= 0.40.1')

atk_bridge_soversion = 0
atk_bridge_libversion = '0.0.0'

cc = meson.get_compiler('c')
host_system = host_machine.system()

# XXX: for strcasecmp(); should use g_ascii_strcasecmp()
add_project_arguments([ '-D_DEFAULT_SOURCE', ], language: 'c')

# XXX: Unused
at_spi_conf = configuration_data()
at_spi_conf.set('GETTEXT_PACKAGE', meson.project_name())
configure_file(output: 'config.h',
               configuration: at_spi_conf)

root_inc = include_directories('.')

# Dependencies
libdbus_req_version = '>= 1.5'
glib_req_version = '>= 2.32.0'
gobject_req_version = '>= 2.0.0'
gmodule_req_version = '>= 2.0.0'
atk_req_version = '>= 2.15.4'
atspi_req_version = '>= 2.17.90'
libxml_req_version = '>= 2.9.1'

libdbus_dep = dependency('dbus-1', version: libdbus_req_version)
glib_dep = dependency('glib-2.0', version: glib_req_version)
gobject_dep = dependency('gobject-2.0', version: gobject_req_version)
gmodule_dep = dependency('gmodule-2.0', version: gmodule_req_version)
atk_dep = dependency('atk', version: atk_req_version)
atspi_dep = dependency('atspi-2', version: atspi_req_version)
libxml_dep = dependency('libxml-2.0', version: libxml_req_version, required: false)

if get_option('disable_p2p')
  p2p_cflags = [ '-DDISABLE_P2P' ]
else
  p2p_cflags = []
endif

import('gnome')

pkg_conf = configuration_data()
pkg_conf.set('prefix', get_option('prefix'))
pkg_conf.set('exec_prefix', get_option('prefix'))
pkg_conf.set('libdir', get_option('libdir'))
pkg_conf.set('includedir', get_option('includedir'))
pkg_conf.set('VERSION', meson.project_version())

configure_file(input: 'atk-bridge-2.0.pc.in',
               output: 'atk-bridge-2.0.pc',
               configuration: pkg_conf,
               install: true,
               install_dir: join_paths(get_option('libdir'), 'pkgconfig'))

install_data('at-spi2-atk.desktop',
             install_dir: join_paths(get_option('libdir'), 'gnome-settings-daemon-3.0', 'gtk-modules'))

subdir('droute')
subdir('atk-adaptor')
subdir('tests')
