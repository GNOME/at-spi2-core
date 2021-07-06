libexec_conf = configuration_data()
libexec_conf.set('libexecdir', atspi_libexecdir)

accessibility_conf = configuration_data()
accessibility_conf.set('DATADIR', atspi_datadir)

if libdbus_dep.version().version_compare('>= 1.12.0')
  accessibility_conf.set('SOCKET_ADDRESS', 'unix:dir=/tmp')
else
  accessibility_conf.set('SOCKET_ADDRESS', 'unix:tmpdir=/tmp')
endif

busconfig_dir = join_paths(atspi_datadir, 'defaults/at-spi2')
session_dir = join_paths(atspi_sysconfdir, 'xdg/autostart')
xwayland_session_dir = join_paths(atspi_sysconfdir, 'xdg/Xwayland-session.d')

configure_file(input: 'accessibility.conf.in',
               output: 'accessibility.conf',
               configuration: accessibility_conf,
               install_dir: busconfig_dir)

configure_file(input: 'at-spi-dbus-bus.desktop.in',
               output: 'at-spi-dbus-bus.desktop',
               configuration: libexec_conf,
               install_dir: session_dir)

configure_file(input: 'org.a11y.Bus.service.in',
               output: 'org.a11y.Bus.service',
               configuration: libexec_conf,
               install_dir: dbus_services_dir)

configure_file(input: 'at-spi-dbus-bus.service.in',
               output: 'at-spi-dbus-bus.service',
               configuration: libexec_conf,
               install_dir: systemd_user_dir)

if x11_dep.found()
  # Note: It is safe to always install it. However, we only need this on
  #       systemd enabled machines where Xwayland may be started on-demand.
  install_data('00-at-spi',
               install_dir: xwayland_session_dir)
endif

launcher_args = [
             '-DSYSCONFDIR="@0@"'.format(atspi_sysconfdir),
             '-DDATADIR="@0@"'.format(atspi_datadir),
           ]

if get_option('dbus_daemon') != 'default'
  launcher_args += '-DDBUS_DAEMON="@0@"'.format(get_option('dbus_daemon'))
else
  dbus_daemon = find_program('dbus-daemon',
                             '/sbin/dbus-daemon',
                             '/usr/sbin/dbus-daemon',
                             '/libexec/dbus-daemon',
                             '/usr/libexec/dbus-daemon',
                             '/usr/lib/dbus-daemon',
                             '/usr/pkg/bin/dbus-daemon',
                             required: false)
  if dbus_daemon.found()
    launcher_args += '-DDBUS_DAEMON="@0@"'.format(dbus_daemon.path())
  endif
endif

needs_systemd = false
if get_option('dbus_broker') != 'default'
  launcher_args += '-DDBUS_BROKER="@0@"'.format(get_option('dbus_broker'))
  needs_systemd = true
else
  dbus_broker = find_program('dbus-broker-launch',
                             required: false)
  if dbus_broker.found()
    launcher_args += '-DDBUS_BROKER="@0@"'.format(dbus_broker.path())
    needs_systemd = true
  endif
endif

if get_option('default_bus') == 'dbus-broker'
  launcher_args += '-DWANT_DBUS_BROKER'
endif

if needs_systemd
  systemd_dep = dependency('libsystemd')
else
  systemd_dep = dependency('', required: false)
endif

executable('at-spi-bus-launcher', 'at-spi-bus-launcher.c',
           include_directories: [ root_inc, include_directories('.') ],
           dependencies: [ gio_dep, systemd_dep, x11_deps ],
           c_args: launcher_args,
           install: true,
           install_dir: atspi_libexecdir)
