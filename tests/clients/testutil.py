#Bus name where test application can be found.
busname = "test.atspi.tree"

#Path of tree interface on test application.
objectpath = "/org/freedesktop/atspi/tree"

#Directory where test data, such as XML results
#can be found.
testdata = "./"

#Location of test application.
testapp = "../apps/test-application"

#Directory of test application modules.
testmodules = "../apps/.libs"

#Atk to at-spi adaptor library location.
atspilib = "../../atk-adaptor/.libs/libspiatk.so"


def runTestApp(module_name):
	import os
	from subprocess import Popen

	testmodule = os.path.join(testmodules, module_name)
	pop = Popen([testapp , "-a", atspilib, "-t", testmodule, "-d", testdata])

	wait_envar = "TEST_APP_WAIT_FOR_DEBUG"

	wait_message = """
	The test application %s has been started with PID %d.
	
	To continue the test press ENTER.\n\n
	"""

	if (wait_envar in os.environ.keys()):
		raw_input(wait_message % (module_name, pop.pid))
