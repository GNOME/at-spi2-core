#!/usr/bin/python

import dbus
import sys
import time
from random import randint

def runTestApp(module_name, dbus_name=None):
	import os
	from subprocess import Popen

	test_data_directory = os.environ["TEST_DATA_DIRECTORY"]
	test_modules_directory = os.environ["TEST_MODULES_DIRECTORY"]
	test_atspi_library = os.environ["TEST_ATSPI_LIBRARY"]
	test_application = os.environ["TEST_APPLICATION"]

	test_module = os.path.join(test_modules_directory, module_name)

	if (dbus_name):
		print " ".join([test_application,
			"--atspi-dbus-name", dbus_name,
			"--test-atspi-library", test_atspi_library,
			"--test-module", test_module,
			"--test-data-directory", test_data_directory,])
		pop = Popen([test_application,
			"--atspi-dbus-name", dbus_name,
			"--test-atspi-library", test_atspi_library,
			"--test-module", test_module,
			"--test-data-directory", test_data_directory,])
	else:
		print " ".join([test_application,
			"--test-atspi-library", test_atspi_library,
			"--test-module", test_module,
			"--test-data-directory", test_data_directory,])
		pop = Popen([test_application,
			"--test-atspi-library", test_atspi_library,
			"--test-module", test_module,
			"--test-data-directory", test_data_directory,])

	wait_message = """
	The test application %s has been started with PID %d.
	
	To continue the test press ENTER.\n\n
	"""
	if ("TEST_APP_WAIT_FOR_DEBUG" in os.environ.keys()):
		raw_input(wait_message % (module_name, pop.pid))

def main(argv):
	testModule = argv[1]

	# TODO Modify this function to add registryd as an option
	bus = dbus.SessionBus()
	name = "test.atspi.R" + str(randint(1, 1000)) 

	app = runTestApp(testModule, name)
	# Wait a little time for the app to start up
	# TODO connect this up to the apps start signal
	time.sleep(1)

	to = bus.get_object(name, "/org/codethink/atspi/test")
	test = dbus.Interface(to, "org.codethink.atspi.test")

	# Run the test script here FIXME

	# Inform the test app it can shut down.
	test.finish()

if __name__=="__main__":
	sys.exit(main(sys.argv))
