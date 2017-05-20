tests = [
  'testdocument',
  'testrole',
  'testrelation',
  'teststateset',
  'testvalue',
]

foreach t: tests
  bin = executable(t, t + '.c',
                   dependencies: libatk_dep,
                   include_directories: root_inc,
                   c_args: common_cflags + [
                     '-DG_DISABLE_DEPRECATED',
                   ])
  test(t, bin, env: [
    'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
    'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  ])
endforeach
