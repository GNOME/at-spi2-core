atspi_introspection_path = ''

registryd_sources = [
  introspection_generated,
  'deviceeventcontroller.c',
  'marshal-dbus.c',
  'reentrant-list.c',
  'registry-main.c',
  'registry.c',
]

registryd_deps = [
  gio_dep,
  libdbus_dep,
  atspi_dep,
  dl_dep,
]

if x11_dep.found()
  registryd_sources += [
    'deviceeventcontroller-x11.c',
    'display.c',
    'event-source.c',
    'ucs2keysym.c',
  ]

  registryd_deps += x11_deps 
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
