dbind_sources = [
  'dbind.c',
  'dbind-any.c',
]

dbind_dep = declare_dependency(sources: dbind_sources,
                               include_directories: root_inc,
                               compile_args: [ '-DG_LOG_DOMAIN="dbind"' ],
                               dependencies: [ libdbus_dep, glib_dep ])

test('dbind-test',
     executable('dbind-test', [ 'dbtest.c', '../atspi/atspi-gmain.c' ],
                include_directories: root_inc,
                dependencies: [ libdbus_dep, glib_dep, dbind_dep ]))
