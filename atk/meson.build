atk_sources = [
  'atkaction.c',
  'atkcomponent.c',
  'atkdocument.c',
  'atkeditabletext.c',
  'atkgobjectaccessible.c',
  'atkhyperlink.c',
  'atkhyperlinkimpl.c',
  'atkhypertext.c',
  'atkimage.c',
  'atknoopobject.c',
  'atknoopobjectfactory.c',
  'atkobject.c',
  'atkobjectfactory.c',
  'atkplug.c',
  'atkprivate.c',
  'atkrange.c',
  'atkregistry.c',
  'atkrelation.c',
  'atkrelationset.c',
  'atkselection.c',
  'atksocket.c',
  'atkstate.c',
  'atkstateset.c',
  'atkstreamablecontent.c',
  'atktable.c',
  'atktablecell.c',
  'atktext.c',
  'atkutil.c',
  'atkmisc.c',
  'atkvalue.c',
  'atkversion.c',
  'atkwindow.c',
]

atk_headers = [
  'atkaction.h',
  'atkcomponent.h',
  'atkdocument.h',
  'atkeditabletext.h',
  'atkgobjectaccessible.h',
  'atkhyperlink.h',
  'atkhyperlinkimpl.h',
  'atkhypertext.h',
  'atkimage.h',
  'atkmisc.h',
  'atknoopobject.h',
  'atknoopobjectfactory.h',
  'atkobject.h',
  'atkobjectfactory.h',
  'atkplug.h',
  'atkrange.h',
  'atkregistry.h',
  'atkrelation.h',
  'atkrelationtype.h',
  'atkrelationset.h',
  'atkselection.h',
  'atksocket.h',
  'atkstate.h',
  'atkstateset.h',
  'atkstreamablecontent.h',
  'atktable.h',
  'atktablecell.h',
  'atktext.h',
  'atkutil.h',
  'atkvalue.h',
  'atkwindow.h',
  'atk-autocleanups.h',
]

install_headers(atk_headers + ['atk.h'], subdir: atk_api_path)

# Features header
atk_version_conf = configuration_data()
atk_version_conf.set('ATK_MAJOR_VERSION', atspi_major_version)
atk_version_conf.set('ATK_MINOR_VERSION', atspi_minor_version)
atk_version_conf.set('ATK_MICRO_VERSION', atspi_micro_version)
atk_version_conf.set('ATK_BINARY_AGE', atk_binary_age)
atk_version_conf.set('ATK_INTERFACE_AGE', atk_interface_age)
atk_version_conf.set('LT_CURRENT_MINUS_AGE', '0')
atk_version_conf.set('ATK_VERSION', atspi_version)

atk_includedir = join_paths(get_option('prefix'), get_option('includedir'))

atk_version_h = configure_file(input: 'atkversion.h.in',
  output: 'atkversion.h',
  configuration: atk_version_conf,
  install_dir: join_paths(atk_includedir, atk_api_path),
)

# Marshallers
atk_marshals = gnome.genmarshal('atkmarshal',
  sources: 'atkmarshal.list',
  prefix: 'atk_marshal',
)
atk_marshal_h = atk_marshals[1]

# Enumerations for GType
atk_enums = gnome.mkenums('atk-enum-types',
  sources: atk_headers,
  c_template: 'atk-enum-types.c.template',
  h_template: 'atk-enum-types.h.template',
  install_dir: join_paths(atk_includedir, atk_api_path),
  install_header: true,
)
atk_enum_h = atk_enums[1]

atk_cflags = [
  '-DG_LOG_DOMAIN="Atk"',
  '-DG_LOG_USE_STRUCTURED=1',
  '-DGLIB_DISABLE_DEPRECATION_WARNINGS',
  '-DATK_DISABLE_DEPRECATION_WARNINGS',
  '-DATK_COMPILATION',
]

if host_system == 'windows'
  atk_win_rc = configure_file(
    input: 'atk.rc.in',
    output: 'atk.rc',
    configuration: atk_version_conf,
  )
  atk_win_res = import('windows').compile_resources(atk_win_rc)
  atk_sources += atk_win_res
endif

libatk = library(atk_api_name,
  sources: atk_sources + atk_enums + atk_marshals,
  soversion: atk_soversion,
  version: atk_libversion,
  install: true,
  dependencies: [glib_dep, gobject_dep],
  include_directories: [ root_inc, atk_inc ],
  c_args: common_cflags + atk_cflags,
  link_args: common_ldflags,
)

atk_sources_dep = [ atk_enum_h ]

if have_gir
  atk_gir = gnome.generate_gir(libatk,
    sources: atk_sources + atk_headers + [ atk_enum_h ] + [ atk_version_h ],
    namespace: 'Atk',
    nsversion: atk_api_version,
    identifier_prefix: 'Atk',
    symbol_prefix: 'atk',
    export_packages: 'atk',
    includes: [ 'GObject-2.0' ],
    install: true,
    extra_args: [
      '--quiet',
      '--c-include=atk/atk.h',
      '-DATK_COMPILATION',
    ],
  )

  atk_sources_dep += atk_gir
endif

libatk_dep = declare_dependency(link_with: libatk,
  include_directories: [ root_inc, atk_inc ],
  dependencies: [glib_dep, gobject_dep],
  sources: atk_sources_dep,
)

pkgconfig.generate(libatk,
  name: 'Atk',
  description: 'Accessibility Toolkit',
  subdirs: atk_api_name,
  filebase: 'atk',
  libraries: [glib_dep, gobject_dep],
)
