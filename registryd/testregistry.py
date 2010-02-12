#!/usr/bin/python

import sys
import dbus
import dbus.service
import gobject

from dbus.mainloop.glib import DBusGMainLoop

DBusGMainLoop(set_as_default=True)

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
		self.obj = self._bus.get_object("org.a11y.atspi.Registry",
                                                "/org/a11y/atspi/registry",
                                                introspect=False)
		self.itf = dbus.Interface(self.obj, dbus_interface="org.a11y.atspi.Registry")
		return self.register_apps

	def register_apps(self):
		#self.itf.registerApplication(":R456", ignore_reply=True)
		#self.itf.registerApplication(":R123", ignore_reply=True)
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
