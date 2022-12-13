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
  'conf.py',
  'de-controller.rst',
  'gitlab-ci.rst',
  'index.rst',
  'roadmap.rst',
  'toolkits.rst',
  'xml-changes.rst',
  'xml-interfaces.rst',
]

# sphinx-build(1) does not seem to easily support generated files in the build directory.
# So, we'll copy all the source files from the source directory to the build directory,
# and just do the build from there.
copied_docs_sources = []
foreach i: docs_sources
  copied_docs_sources += fs.copyfile(i)
endforeach

custom_target(
  'devel_docs',
  input: copied_docs_sources + interfaces_rst,
  output: 'html',
  command: [ sphinx_build, meson.current_build_dir(), '@OUTPUT@' ],
)
