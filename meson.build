project('at-spi2-core', 'c',
        version: '2.36.1',
        license: 'LGPLv2.1+',
        default_options: [
          'buildtype=debugoptimized',
          'warning_level=1',
          'c_std=c99',
        ],
        meson_version: '>= 0.40.1')

add_project_arguments([ '-D_POSIX_C_SOURCE=200809L', '-D_DEFAULT_SOURCE' ], language: 'c')

atspi_gir_ns = 'Atspi'

cc = meson.get_compiler('c')
host_system = host_machine.system()

soversion = '0.0.1'

at_spi_conf = configuration_data()
at_spi_conf.set('GETTEXT_PACKAGE', meson.project_name())

root_inc = include_directories('.')
registryd_inc = include_directories('registryd')

atspi_prefix = get_option('prefix')
atspi_datadir = join_paths(atspi_prefix, get_option('datadir'))
atspi_libexecdir = join_paths(atspi_prefix, get_option('libexecdir'))
atspi_sysconfdir = join_paths(atspi_prefix, get_option('sysconfdir'))
atspi_libdir = join_paths(atspi_prefix, get_option('libdir'))
atspi_includedir = join_paths(atspi_prefix, get_option('includedir'))

if get_option('dbus_services_dir') != 'default'
  dbus_services_dir = get_option('dbus_services_dir')
else
  dbus_services_dir = join_paths(get_option('datadir'), 'dbus-1/services')
endif

if get_option('systemd_user_dir') != 'default'
  systemd_user_dir = get_option('systemd_user_dir')
else
  systemd_user_dir = join_paths(get_option('prefix'), 'lib/systemd/user')
endif

# Dependencies
libdbus_req_version = '>= 1.5'
glib_req_version = '>= 2.32.0'
gobject_req_version = '>= 2.0.0'
gio_req_version = '>= 2.28.0'

libdbus_dep = dependency('dbus-1', version: libdbus_req_version)
glib_dep = dependency('glib-2.0', version: glib_req_version)
gobject_dep = dependency('gobject-2.0', version: gobject_req_version)
gio_dep = dependency('gio-2.0', version: gio_req_version)
if cc.has_function('dlopen')
  dl_dep = []
elif cc.has_function('dlopen', args: '-ldl')
  dl_dep = cc.find_library('dl')
else
  error('Could not find a library with the dlopen function')
endif

x11_deps = []
x11_option = get_option('x11')
if x11_option != 'no'
  x11_dep = dependency('x11', required: false)

  if x11_dep.found()
    x11_deps += x11_dep
    at_spi_conf.set('HAVE_X11', 1)
    xtest_dep = dependency('xtst')
    x11_deps += xtest_dep

    if cc.has_function('XkbGetMap', dependencies: x11_deps)
      at_spi_conf.set('HAVE_XKB', 1)
    endif

    xinput_dep = dependency('xi')
    x11_deps += xinput_dep
  endif
endif

# Alignments
at_spi_conf.set('ALIGNOF_CHAR', cc.alignment('char'))
at_spi_conf.set('ALIGNOF_DOUBLE', cc.alignment('double'))

dbus_alignments = [
  'dbus_bool_t',
  'dbus_int16_t',
  'dbus_int32_t',
  'dbus_int64_t',
]

foreach a: dbus_alignments
  at_spi_conf.set('ALIGNOF_' + a.underscorify().to_upper(),
                  cc.alignment(a, prefix: '#include <dbus/dbus.h>', dependencies: libdbus_dep))
endforeach

at_spi_conf.set('ALIGNOF_DBIND_POINTER', cc.alignment('dbind_pointer', prefix: 'typedef void *dbind_pointer;'))
at_spi_conf.set('ALIGNOF_DBIND_STRUCT', cc.alignment('dbind_struct', prefix: 'typedef struct { char s1; } dbind_struct;'))

# introspection support
have_gir = false

introspection_option = get_option('introspection')
if introspection_option != 'no'
  gir_dep = dependency('gobject-introspection-1.0', version: '>= 0.6.7', required: false)

  if gir_dep.found()
    have_gir = true
  endif
endif

xgettext = find_program('xgettext', required : false)

configure_file(output: 'config.h', configuration: at_spi_conf)

gnome = import('gnome')

subdir('dbind')
subdir('atspi')
subdir('bus')
subdir('registryd')
subdir('test')

if get_option('docs')
  subdir('doc/libatspi')
endif

if xgettext.found()
  subdir('po')
endif
