dbind_sources = [
  'dbind.c',
  'dbind-any.c',
]

dbind = static_library('dbind', dbind_sources,
                       include_directories: root_inc,
                       dependencies: [ libdbus_dep, glib_dep ],
                       c_args: [ '-DG_LOG_DOMAIN="dbind"' ])

dbind_dep = declare_dependency(link_with: dbind)

test('dbind-test',
     executable('dbind-test', [ 'dbtest.c', '../atspi/atspi-gmain.c' ],
                include_directories: root_inc,
                dependencies: [ libdbus_dep, glib_dep, dbind_dep ]))
