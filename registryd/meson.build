atspi_introspection_path = ''

registryd_sources = [
  'de-marshaller.c',
  'registry-main.c',
  'registry.c',
  'introspection.c',
  'deviceeventcontroller.c',
  'reentrant-list.c',
]

registryd_deps = [
  gio_dep,
  libdbus_dep,
  atspi_dep,
  dl_dep,
]

x11_option = get_option('x11')
if x11_option != 'no'
  if x11_dep.found()
    registryd_sources += [
      'deviceeventcontroller-x11.c',
      'display.c',
      'event-source.c',
      'ucs2keysym.c',
    ]

    registryd_deps += x11_deps 
  endif
endif

executable('at-spi2-registryd', registryd_sources,
           dependencies: registryd_deps,
           install: true,
           install_dir: atspi_libexecdir)

libexec_conf = configuration_data()
libexec_conf.set('libexecdir', atspi_libexecdir)

configure_file(input: 'org.a11y.atspi.Registry.service.in',
               output: 'org.a11y.atspi.Registry.service',
               configuration: libexec_conf,
               install_dir: join_paths(atspi_datadir, 'dbus-1/accessibility-services'))
