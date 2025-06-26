gdbus_codegen = find_program('gdbus-codegen',
                             native: true,
                             required: get_option('docs'))

sphinx_build = find_program('sphinx-build',
                            native: true,
                            required: get_option('docs'))

# Gather the XML files, prefixed by the "xml/" directory name
# introspection_sources comes from xml/meson.build
interfaces_xml = []
foreach i: introspection_sources
  interfaces_xml += join_paths(meson.project_source_root(), 'xml', i)
endforeach

# gdbus-codegen can take an dbus.xml file and generate various interfaces
# from it, named as some_prefix-org.foo.InterfaceName.rst
# We need an explicit list of those; for example, xml/Event.xml contains
# various interfaces and so we can't just substitute "xml" for "rst".
interfaces_rst_sources = [
  'doc-org.a11y.atspi.Accessible.rst',
  'doc-org.a11y.atspi.Action.rst',
  'doc-org.a11y.atspi.Application.rst',
  'doc-org.a11y.atspi.Cache.rst',
  'doc-org.a11y.atspi.Collection.rst',
  'doc-org.a11y.atspi.Component.rst',
  'doc-org.a11y.atspi.DeviceEventController.rst',
  'doc-org.a11y.atspi.DeviceEventListener.rst',
  'doc-org.a11y.atspi.Document.rst',
  'doc-org.a11y.atspi.EditableText.rst',
  'doc-org.a11y.atspi.Event.Document.rst',
  'doc-org.a11y.atspi.Event.Focus.rst',
  'doc-org.a11y.atspi.Event.Keyboard.rst',
  'doc-org.a11y.atspi.Event.Mouse.rst',
  'doc-org.a11y.atspi.Event.Object.rst',
  'doc-org.a11y.atspi.Event.Terminal.rst',
  'doc-org.a11y.atspi.Event.Window.rst',
  'doc-org.a11y.atspi.Hyperlink.rst',
  'doc-org.a11y.atspi.Hypertext.rst',
  'doc-org.a11y.atspi.Image.rst',
  'doc-org.a11y.atspi.Registry.rst',
  'doc-org.a11y.atspi.Selection.rst',
  'doc-org.a11y.atspi.Socket.rst',
  'doc-org.a11y.atspi.Table.rst',
  'doc-org.a11y.atspi.TableCell.rst',
  'doc-org.a11y.atspi.Text.rst',
  'doc-org.a11y.atspi.Value.rst',
]

# The 'devel-docs/doc' here is a prefix that gdbus-codegen will put in front
# of the interface names that it finds in the input XML files.
interfaces_rst = custom_target(
  'interfaces_rst',
  input: interfaces_xml,
  output: interfaces_rst_sources,
  command: [ gdbus_codegen, '--generate-rst', 'devel-docs/doc', '@INPUT@']
)

docs_sources = [
  'architecture.rst',
  'architecture-modules.svg',
  'at-spi3.rst',
  'atk-deprecations.rst',
  'atspi-python-stack.rst',
  'conf.py',
  'de-controller.rst',
  'gitlab-ci.rst',
  'index.rst',
  'meeting-2023-01-13.rst',
  'new-protocol.rst',
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
