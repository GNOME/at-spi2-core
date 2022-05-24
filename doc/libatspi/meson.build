version_xml_conf = configuration_data()
version_xml_conf.set('PACKAGE_VERSION', meson.project_name())
configure_file(input: 'version.xml.in',
               output: 'version.xml',
               configuration: version_xml_conf)

glib_prefix = dependency('glib-2.0').get_variable(pkgconfig: 'prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(atspi_datadir, 'gtk-doc', 'html')

gnome.gtkdoc('libatspi',
             main_xml: 'libatspi-docs.sgml',
             src_dir: [
               join_paths(meson.project_source_root(), 'atspi'),
               join_paths(meson.project_build_root(), 'atspi'),
             ],
             dependencies: atspi_dep,
             gobject_typesfile: 'libatspi.types',
             scan_args: [
               '--rebuild-types',
             ],
             mkdb_args: [
               '--sgml-mode',
               '--output-format=xml',
             ],
             fixxref_args: [
               '--html-dir=@0@'.format(docpath),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
             ],
             install: true)

