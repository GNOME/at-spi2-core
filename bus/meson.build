libexec_conf = configuration_data()
libexec_conf.set('libexecdir', atspi_libexecdir)

accessibility_conf = configuration_data()
accessibility_conf.set('DATADIR', atspi_datadir)

busconfig_dir = join_paths(atspi_datadir, 'defaults/at-spi2')
session_dir = join_paths(atspi_sysconfdir, 'xdg/autostart')

configure_file(input: 'accessibility.conf.in',
               output: 'accessibility.conf',
               configuration: accessibility_conf,
               install: true,
               install_dir: busconfig_dir)

configure_file(input: 'at-spi-dbus-bus.desktop.in',
               output: 'at-spi-dbus-bus.desktop',
               configuration: libexec_conf,
               install: true,
               install_dir: session_dir)

configure_file(input: 'org.a11y.Bus.service.in',
               output: 'org.a11y.Bus.service',
               configuration: libexec_conf,
               install: true,
               install_dir: dbus_services_dir)

configure_file(input: 'at-spi-dbus-bus.service.in',
               output: 'at-spi-dbus-bus.service',
               configuration: libexec_conf,
               install: true,
               install_dir: systemd_user_dir)

if get_option('dbus_daemon') != 'default'
  dbus_daemon = get_option('dbus_daemon')
else
  dbus_daemon = find_program('dbus-daemon',
                             '/sbin/dbus-daemon',
                             '/usr/sbin/dbus-daemon',
                             '/libexec/dbus-daemon',
                             '/usr/libexec/dbus-daemon',
                             '/usr/pkg/bin/dbus-daemon',
                             required: true).path()
endif

executable('at-spi-bus-launcher', 'at-spi-bus-launcher.c',
           include_directories: [ root_inc, include_directories('.') ],
           dependencies: [ gio_dep, x11_deps ],
           c_args: [
             '-DSYSCONFDIR="@0@"'.format(atspi_sysconfdir),
             '-DDATADIR="@0@"'.format(atspi_datadir),
             '-DDBUS_DAEMON="@0@"'.format(dbus_daemon),
           ],
           install: true,
           install_dir: atspi_libexecdir)
