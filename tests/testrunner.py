import sys
import unittest
import clients

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-d", "--testdata", dest="testdata", help="Location of test data directory")
parser.add_option("-a", "--atspilib", dest="atspilib", help="Location of atk to atspi adaptor")
parser.add_option("-t", "--testmodules", dest="testmodules", help="Location of test application libraries")
parser.add_option("-r", "--testapp", dest="testapp", help="Location of test application executable")
parser.add_option("-b", "--busname", dest="busname", help="D-Bus name of test application")
parser.add_option("-o", "--objectpath", dest="objectpath", help="Object path of test applicaiton tree interface")

optionvars = ["testdata",
		"atspilib",
		"testmodules",
		"testapp",
		"busname",
		"objectpath"]

def set_data(options, name):
	val = getattr(options, name)
	if val is not None:
		setattr(clients.testutil, name, val)

def main(argv):
	(options, args) = parser.parse_args()
	[set_data(options, name) for name in optionvars]

	runner = unittest.TextTestRunner()
	testsuite = unittest.defaultTestLoader.loadTestsFromModule(clients)
	runner.run(testsuite)

if __name__=="__main__":
	sys.exit(main(sys.argv))
