droute_sources = [
  'droute.c',
  'droute-variant.c',
  'droute-pairhash.c',
]

libdroute = static_library('droute', droute_sources,
                           dependencies: [ glib_dep, libdbus_dep ],
                           include_directories: root_inc)

libdroute_dep = declare_dependency(link_with: libdroute,
                                   dependencies: [ glib_dep, libdbus_dep ],
                                   include_directories: include_directories('.'))

droute_test = executable('droute-test', 'droute-test.c',
                         dependencies: [ libdroute_dep, atspi_dep ],
                         include_directories: root_inc)
test('droute-test', droute_test)
