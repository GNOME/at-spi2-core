
def runTestApp(module_name, dbus_name):
	import os
	from subprocess import Popen

	test_data_directory = os.environ["TEST_DATA_DIRECTORY"]
	test_modules_directory = os.environ["TEST_MODULES_DIRECTORY"]
	test_atspi_library = os.environ["TEST_ATSPI_LIBRARY"]
	test_application = os.environ["TEST_APPLICATION"]

	test_module = os.path.join(test_modules_directory, module_name)
	pop = Popen([test_application,
		"--atspi-dbus-name", dbus_name,
		"--test-atspi-library", test_atspi_library,
		"--test-module", test_module,
		"--test-data-directory", test_data_directory,])

	print " ".join([test_application,
		"--atspi-dbus-name", dbus_name,
		"--test-atspi-library", test_atspi_library,
		"--test-module", test_module,
		"--test-data-directory", test_data_directory,])

	wait_message = """
	The test application %s has been started with PID %d.
	
	To continue the test press ENTER.\n\n
	"""
	if ("TEST_APP_WAIT_FOR_DEBUG" in os.environ.keys()):
		raw_input(wait_message % (module_name, pop.pid))
