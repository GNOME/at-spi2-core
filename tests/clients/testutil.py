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

from subprocess import Popen
from os.path import join

def runTestApp(module_name):
	testmodule = join(testmodules, module_name)
	print testmodule
	print testapp
	return Popen([testapp , "-a", atspilib, "-t", testmodule, "-d", testdata])
