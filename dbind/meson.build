dbind_sources = [
  'dbind.c',
  'dbind-any.c',
]

dbind_test_sources = [ 'dbtest.c']
if not dbus_glib_dep.found()
  dbind_test_sources += ['../atspi/atspi-gmain.c']
endif

dbind_dep = declare_dependency(sources: dbind_sources,
                               include_directories: root_inc,
                               compile_args: [ '-DG_LOG_DOMAIN="atspi"' ],
                               dependencies: [ libdbus_dep, glib_dep, dbus_glib_dep ])

test('dbind-test',
     executable('dbind-test',
                sources: dbind_test_sources,
                include_directories: root_inc,
                dependencies: [ libdbus_dep, glib_dep, dbind_dep ]))
