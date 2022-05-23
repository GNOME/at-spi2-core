subdir('xml')

private_headers = [
  'atkmarshal.h',
  'atkintl.h',
]

glib_prefix = dependency('glib-2.0').get_variable(pkgconfig: 'prefix')
glib_docpath = glib_prefix / 'share/gtk-doc/html'
docpath = atk_datadir / 'gtk-doc/html'

gnome.gtkdoc('atk',
  main_xml: 'atk-docs.xml',
  src_dir: atk_inc,
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
  install: true,
)
