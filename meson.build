project('at-spi2-core', 'c',
        version: '2.58.0',
        license: 'LGPLv2.1+',
        default_options: [
          'buildtype=debugoptimized',
          'warning_level=1',
          'c_std=c99',
        ],
        meson_version: '>= 0.64.0')

add_project_arguments([ '-D_XOPEN_SOURCE=700', '-D_GNU_SOURCE' ], language: 'c')

fs = import('fs')

atspi_gir_ns = 'Atspi'

cc = meson.get_compiler('c')
host_system = host_machine.system()

soversion = '0.0.1'

atspi_version = meson.project_version()
version = atspi_version.split('.')
atspi_major_version = version[0].to_int()
atspi_minor_version = version[1].to_int()
atspi_micro_version = version[2].to_int()

atk_interface_age = 1
atk_binary_age = 10000 * atspi_major_version + 100 * atspi_minor_version + 10 + atspi_micro_version

atk_api_version = '1.0'
atk_api_name = 'atk-@0@'.format(atk_api_version)
atk_api_path = '@0@/atk'.format(atk_api_name)

# Maintain version scheme with libtool
atk_soversion = 0
current = atk_binary_age - atk_interface_age
atk_libversion = '@0@.@1@.@2@'.format(atk_soversion, current, atk_interface_age)
darwin_versions = ['@0@'.format(current + 1), '@0@.@1@'.format(current + 1, atk_interface_age)]

add_project_arguments([ '-DG_DISABLE_SINGLE_INCLUDES', '-DATK_DISABLE_SINGLE_INCLUDES' ], language: 'c')

if cc.get_id() == 'msvc'
  add_project_arguments(cc.get_supported_arguments(['-FImsvc_recommended_pragmas.h', '-utf-8']), language: 'c')
endif

# at-spi2-atk versions
atk_bridge_version = meson.project_version()
atk_bridge_soversion = 0
atk_bridge_libversion = '0.0.0'

# Compiler and linker flags
common_cflags = []
common_ldflags = []

test_cflags = []

at_spi_conf = configuration_data()
at_spi_conf.set_quoted('ATK_VERSION', atspi_version)
at_spi_conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())

# Symbol visibility
if get_option('default_library') != 'static'
  if host_system == 'windows'
    at_spi_conf.set('DLL_EXPORT', true)
    at_spi_conf.set('_ATK_EXTERN', '__declspec(dllexport) extern')
    if cc.get_id() != 'msvc'
      test_cflags += ['-fvisibility=hidden']
    endif
  else
    at_spi_conf.set('_ATK_EXTERN', '__attribute__((visibility("default"))) extern')
    test_cflags += ['-fvisibility=hidden']
  endif
endif

# Check all compiler flags
common_cflags += cc.get_supported_arguments(test_cflags)

# Linker flags
if host_machine.system() == 'linux'
  test_ldflags = [ '-Wl,-Bsymbolic', '-Wl,-z,relro', '-Wl,-z,now', ]
  common_ldflags += cc.get_supported_link_arguments(test_ldflags)
endif

# Maintain compatibility with autotools on macOS
if host_machine.system() == 'darwin'
  common_ldflags += [ '-compatibility_version', darwin_versions[0], '-current_version', darwin_versions[1]]
  add_project_arguments([ '-D_DARWIN_C_SOURCE' ], language: 'c')
endif

# Functions
checked_funcs = [
  'bind_textdomain_codeset',
]

foreach f: checked_funcs
  if cc.has_function(f)
    at_spi_conf.set('HAVE_' + f.underscorify().to_upper(), 1)
  endif
endforeach

root_inc = include_directories('.')
atk_inc = include_directories('atk')
registryd_inc = include_directories('registryd')

atspi_prefix = get_option('prefix')
atspi_datadir = join_paths(atspi_prefix, get_option('datadir'))
atspi_libexecdir = join_paths(atspi_prefix, get_option('libexecdir'))
atspi_sysconfdir = join_paths(atspi_prefix, get_option('sysconfdir'))

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

at_spi_conf.set_quoted('ATSPI_LOCALEDIR', join_paths(atspi_datadir, 'locale'))

# Dependencies
libdbus_req_version = '>= 1.5'
glib_req_version = '>= 2.67.4'
gobject_req_version = '>= 2.0.0'
gmodule_req_version = '>= 2.0.0'
gio_req_version = '>= 2.28.0'
libxml_req_version = '>= 2.9.1'

if not get_option('atk_only')
  libdbus_dep = dependency('dbus-1', version: libdbus_req_version)
endif

glib_dep = dependency('glib-2.0', version: glib_req_version)
gobject_dep = dependency('gobject-2.0', version: gobject_req_version)
gio_dep = dependency('gio-2.0', version: gio_req_version)
if not get_option('atk_only')
  if cc.has_function('dlopen')
    dl_dep = []
  elif cc.has_function('dlopen', args: '-ldl')
    dl_dep = cc.find_library('dl')
  else
    error('Could not find a library with the dlopen function')
  endif
endif
gmodule_dep = dependency('gmodule-2.0', version: gmodule_req_version)
libxml_dep = dependency('libxml-2.0', version: libxml_req_version)

x11_deps = []
x11_dep = dependency('x11', required: get_option('x11'))
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

# Alignments
at_spi_conf.set('ALIGNOF_CHAR', cc.alignment('char'))
at_spi_conf.set('ALIGNOF_DOUBLE', cc.alignment('double'))

dbus_alignments = [
  'dbus_bool_t',
  'dbus_int16_t',
  'dbus_int32_t',
  'dbus_int64_t',
]

if not get_option('atk_only')
  foreach a: dbus_alignments
    at_spi_conf.set('ALIGNOF_' + a.underscorify().to_upper(),
                    cc.alignment(a, prefix: '#include <dbus/dbus.h>', dependencies: libdbus_dep))
  endforeach
endif

at_spi_conf.set('ALIGNOF_DBIND_POINTER', cc.alignment('dbind_pointer', prefix: 'typedef void *dbind_pointer;'))
at_spi_conf.set('ALIGNOF_DBIND_STRUCT', cc.alignment('dbind_struct', prefix: 'typedef struct { char s1; } dbind_struct;'))

# introspection support
gir = find_program('g-ir-scanner', required : get_option('introspection'))
have_gir = gir.found() and (not meson.is_cross_build() or get_option('introspection').enabled())

if not get_option('atk_only')
  pymod = import('python')
  python = pymod.find_installation(get_option('python'), pure: false)
endif

xgettext = find_program('xgettext', required : false)

configure_file(output: 'config.h', configuration: at_spi_conf)

pkgconfig = import('pkgconfig')
gnome = import('gnome')

subdir('atk')

if not get_option('atk_only')
  install_data('at-spi2-atk.desktop',
               install_dir: join_paths(get_option('libdir'), 'gnome-settings-daemon-3.0', 'gtk-modules'))

  subdir('xml')
  subdir('dbind')
  subdir('atspi')
  subdir('bus')
  subdir('registryd')
  subdir('droute')
  subdir('atk-adaptor')
  subdir('tests')
  if python.found()
    subdir('python')
  endif
endif

if get_option('docs')
  subdir('doc')
  subdir('devel-docs')
endif

if xgettext.found()
  subdir('po')
endif
