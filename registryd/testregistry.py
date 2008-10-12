#!/usr/bin/python

import sys
import dbus
import dbus.service
import gobject

from dbus.mainloop.glib import DBusGMainLoop

DBusGMainLoop(set_as_default=True)

class Emitter (dbus.service.Object):
	def __init__(self, bus, object_path):
		dbus.service.Object.__init__(self, bus, object_path)

	@dbus.service.signal(dbus_interface='org.freedekstop.atspi.Tree', signature='s')
	def registerApplication(self, app_name):
		pass

	@dbus.service.signal(dbus_interface='org.freedekstop.atspi.Tree', signature='s')
	def deregisterApplication(self, app_name):
		pass

class IdleStateM (object):
	def __init__(self, bus, loop):
		self._bus = bus
		self._loop = loop
		self._func = self.setup

	def idle_handler(self):
		self._func = self._func()
		if self._func == None:
			self._func = self.teardown
		return True

	def setup(self):
		self.emit = Emitter(self._bus, "/org/freedesktop/atspi/tree")
		self.obj = self._bus.get_object("org.freedesktop.atspi.Registry", "/org/freedesktop/atspi/Registry")
		self.itf = dbus.Interface(self.obj, dbus_interface="org.freedesktop.atspi.Registry")
		return self.emit_registers

	def emit_registers(self):
		self.emit.registerApplication(":R123")
		self.emit.registerApplication(":R456")
		return self.print_applications

	def print_applications(self):
		apps = self.itf.getApplications()
		print apps
		return self.teardown

	def teardown(self):
		self._loop.quit()

def main(argv):
	bus = dbus.SessionBus()
	loop = gobject.MainLoop()
	stateM = IdleStateM(bus, loop)
	gobject.idle_add(stateM.idle_handler)
	loop.run()

if __name__=="__main__":
	sys.exit(main(sys.argv))
