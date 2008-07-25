#!/usr/bin/python

import sys
import unittest
import clients

def main(argv):
	runner = unittest.TextTestRunner()
	testsuite = unittest.defaultTestLoader.loadTestsFromModule(clients)
	result = runner.run(testsuite)

	if result.wasSuccessful():
		return 0
	else:
		return 1

if __name__=="__main__":
	sys.exit(main(sys.argv))
