subdir('xml')

private_headers = [
  'atkmarshal.h',
  'atkintl.h',
]

content_files = [
]

html_images = [
]

glib_prefix = dependency('glib-2.0').get_pkgconfig_variable('prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(atk_datadir, 'gtk-doc', 'html')

gnome.gtkdoc('atk',
             main_xml: 'atk-docs.xml',
             src_dir: [
               join_paths(meson.source_root(), 'atk'),
               join_paths(meson.build_root(), 'atk'),
             ],
             dependencies: libatk_dep,
             gobject_typesfile: 'atk.types',
             scan_args: [
               '--rebuild-types',
               '--deprecated-guards=ATK_DISABLE_DEPRECATED',
               '--ignore-decorators=_ATK_EXTERN',
               '--ignore-headers=' + ' '.join(private_headers),
             ],
             mkdb_args: [
               '--output-format=xml',
             ],
             fixxref_args: [
               '--html-dir=@0@'.format(docpath),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
             ],
             html_assets: html_images,
             install: true)
