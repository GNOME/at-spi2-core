atspi_sources = [
  'atspi-accessible.c',
  'atspi-action.c',
  'atspi-application.c',
  'atspi-collection.c',
  'atspi-component.c',
  'atspi-device-listener.c',
  'atspi-document.c',
  'atspi-editabletext.c',
  'atspi-event-listener.c',
  'atspi-gmain.c',
  'atspi-hyperlink.c',
  'atspi-hypertext.c',
  'atspi-image.c',
  'atspi-matchrule.c',
  'atspi-misc.c',
  'atspi-object.c',
  'atspi-registry.c',
  'atspi-relation.c',
  'atspi-selection.c',
  'atspi-stateset.c',
  'atspi-table.c',
  'atspi-table-cell.c',
  'atspi-text.c',
  'atspi-value.c',
]

atspi_headers = [
  'atspi.h',
  'atspi-accessible.h',
  'atspi-action.h',
  'atspi-application.h',
  'atspi-collection.h',
  'atspi-component.h',
  'atspi-constants.h',
  'atspi-device-listener.h',
  'atspi-document.h',
  'atspi-editabletext.h',
  'atspi-event-listener.h',
  'atspi-gmain.h',
  'atspi-hyperlink.h',
  'atspi-hypertext.h',
  'atspi-image.h',
  'atspi-matchrule.h',
  'atspi-misc.h',
  'atspi-object.h',
  'atspi-registry.h',
  'atspi-relation.h',
  'atspi-selection.h',
  'atspi-stateset.h',
  'atspi-table.h',
  'atspi-table-cell.h',
  'atspi-text.h',
  'atspi-types.h',
  'atspi-value.h',
]

atspi_includedir = join_paths(get_option('prefix'), get_option('includedir'), 'at-spi-2.0', 'atspi')

install_headers(atspi_headers, subdir: atspi_includedir)

atspi_enums = gnome.mkenums('atspi-enum-types',
                            sources: [ 'atspi-constants.h', 'atspi-types.h' ],
                            c_template: 'atspi-enum-types.c.template',
                            h_template: 'atspi-enum-types.h.template',
                            install_dir: atspi_includedir,
                            install_header: true)
atspi_enum_h = atspi_enums[1]

atspi = shared_library('atspi', atspi_sources + atspi_enums,
                       include_directories: [ root_inc, registryd_inc ],
                       dependencies: [ libdbus_dep, gobject_dep, dbind_dep ],
                       install: true)

atspi_dep = declare_dependency(link_with: atspi,
                               sources: atspi_enum_h,
                               include_directories: root_inc,
                               dependencies: [ libdbus_dep, gobject_dep, ])

if have_gir
  gir_sources = atspi_sources + atspi_enums + atspi_headers

  gir_incs = [
    'DBus-1.0',
    'GLib-2.0',
    'GObject-2.0'
  ]

  gir_extra_args = [
    '--c-include=atspi/atspi.h',
    '--pkg-export=atspi-2'
  ]

  gir_dir = join_paths(atspi_datadir, 'gir-1.0')
  typelib_dir = join_paths(get_option('libdir'), 'girepository-1.0')

  libatspi_gir = gnome.generate_gir(
    atspi,
    sources: gir_sources,
    nsversion: '2.0',
    namespace: atspi_gir_ns,
    symbol_prefix: atspi_gir_ns.to_lower(),
    identifier_prefix: atspi_gir_ns,
    includes: gir_incs,
    include_directories: root_inc,
    install: true,
    install_dir_gir: gir_dir,
    install_dir_typelib: typelib_dir,
    extra_args: gir_extra_args
  )
endif

