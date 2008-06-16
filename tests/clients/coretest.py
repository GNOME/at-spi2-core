import unittest
import testutil

import dbus
import gobject
import os.path
from dbus.mainloop.glib import DBusGMainLoop

from accessible_cache import AccessibleCache
from random import randint



class CoreTestCase(unittest.TestCase):
	def setUp(self):
		DBusGMainLoop(set_as_default=True)

		self._bus = dbus.SessionBus()
		self._loop = gobject.MainLoop()
		self._name = "test.atspi.R" + str(randint(1, 1000)) 
		self._match = self._bus.add_signal_receiver(self.post_application_setup,
							"started",
							"org.codethink.atspi.test",
							self._name)
		self._started = False

		if "TEST_APP_WAIT_FOR_DEBUG" not in os.environ.keys():
			gobject.timeout_add(1000, self.application_check)

	def tearDown(self):
		#Shut down the test application
		self._test.finished()

		del(self._bus)
		del(self._loop)
		del(self._cache)
		del(self._app)
		del(self._test)

	def application_check(self):
		if not self._started:
			self.fail("Test application did not start")

	def post_application_setup(self):
		self._started = True

		self._cache = AccessibleCache(self._bus, self._name, "/org/freedesktop/atspi/tree")

		test_object = self._bus.get_object(self._name, "/org/codethink/atspi/test")
		self._test = dbus.Interface(test_object, "org.codethink.atspi.test")

		self.post_application_test()

		self._loop.quit()

	def post_application_test(self):
		raise  Exception, "No test has been defined"
