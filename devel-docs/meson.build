gdbus_codegen = find_program('gdbus-codegen',
                             native: true,
                             required: get_option('docs'))

sphinx_build = find_program('sphinx-build',
                            native: true,
                            required: get_option('docs'))

interfaces_rst_sources = []
interfaces_xml = []

foreach i: introspection_sources
  # /foo/bar/baz.xml -> doc-baz.rst
  doc_name = 'doc-' + fs.replace_suffix(fs.name(i), '.rst')
  interfaces_rst_sources += doc_name

  interfaces_xml += join_paths(meson.project_source_root(), 'xml', i)
endforeach

interfaces_rst = custom_target(
  'interfaces_rst',
  input: interfaces_xml,
  output: interfaces_rst_sources,
  command: [ gdbus_codegen, '--generate-rst', 'devel-docs/doc', '@INPUT@']
)

docs_sources = [
  'atk-deprecations.rst',
  'de-controller.rst',
  'gitlab-ci.rst',
  'index.rst',
  'roadmap.rst',
  'toolkits.rst',
  'xml-changes.rst',
  'xml-interfaces.rst',
]

custom_target(
  'devel_docs',
  input: docs_sources + interfaces_rst,
  output: 'devel-docs',
  command: [ sphinx_build, meson.current_source_dir(), '@OUTPUT@' ],
  env: { 'MESON_CURRENT_BUILD_DIR': meson.current_build_dir() }
)
