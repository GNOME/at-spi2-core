testapp = executable('test-application',
                     'test-application.c',
                     include_directories: root_inc,
                     dependencies: [ atspi_dep ],
                    )

memory = executable('memory',
                    'memory.c',
                    include_directories: root_inc,
                    dependencies: [ atspi_dep ],
                   )

test(
  'memory',
  memory,
  depends: testapp,
  is_parallel: false,
)
