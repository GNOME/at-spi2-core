dependency('gi-docgen', version: '>= 2021.1',
           fallback: ['gi-docgen', 'dummy_dep'],
           native: true,
           required: get_option('docs'))

gidocgen = find_program('gi-docgen')

docs_dir = atspi_datadir / 'doc'

gidocgen_common_args = [
  '--quiet',
  '--no-namespace-dir',
  '--fatal-warnings',
]

################################ ATK
atk_expand_content_files =[]

atk_toml_data = configuration_data()
atk_toml_data.set('VERSION', meson.project_version())

atk_toml = configure_file(
  input: 'atk.toml.in',
  output: 'atk.toml',
  configuration: atk_toml_data
)

custom_target('atk-doc',
  input: [ atk_toml, atk_gir[0] ],
  output: 'atk',
  command: [
    gidocgen,
    'generate',
    gidocgen_common_args,
    '--config=@INPUT0@',
    '--output-dir=@OUTPUT@',
    '--content-dir=@0@'.format(meson.current_source_dir()),
    '@INPUT1@',
  ],
  install: true,
  install_dir: docs_dir,
  depend_files: atk_expand_content_files,
)


################################ libatspi
libatspi_expand_content_files = []

libatspi_toml_data = configuration_data()
libatspi_toml_data.set('VERSION', meson.project_version())

libatspi_toml = configure_file(
  input: 'libatspi.toml.in',
  output: 'libatspi.toml',
  configuration: libatspi_toml_data
)

custom_target('libatspi-doc',
  input: [ libatspi_toml, libatspi_gir[0] ],
  output: 'libatspi',
  command: [
    gidocgen,
    'generate',
    gidocgen_common_args,
    '--config=@INPUT0@',
    '--output-dir=@OUTPUT@',
    '--content-dir=@0@'.format(meson.current_source_dir()),
    '@INPUT1@',
  ],
  install: true,
  install_dir: docs_dir,
  depend_files: libatspi_expand_content_files,
)

