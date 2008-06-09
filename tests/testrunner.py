#!/usr/bin/python

import sys
import os
import unittest
import clients

optionvars = ["testdata",
		"atspilib",
		"testmodules",
		"testapp",
		"busname",
		"objectpath"]


def main(argv):
	def set_data(name):
		if name in os.environ.keys():
			setattr(clients.testutil, name, os.environ[name])

	[set_data(name) for name in optionvars]

	runner = unittest.TextTestRunner()
	testsuite = unittest.defaultTestLoader.loadTestsFromModule(clients)
	result = runner.run(testsuite)

	if result.wasSuccessful():
		return 0
	else:
		return 1


if __name__=="__main__":
	sys.exit(main(sys.argv))
