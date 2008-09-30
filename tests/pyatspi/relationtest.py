import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi

class RelationTest(_PasyTest):

	__tests__ = ["setup",
		     "teardown",
		     "test_getRelationType",
		     "test_getRelationTypeName",
		     "test_getNTargets",
		     "test_getTarget",
		     ]

	def __init__(self, bus, path):
		_PasyTest.__init__(self, "Relation", False)
		self._bus = bus
		self._path = path

	def setup(self, test):
		self._registry = pyatspi.Registry()
		self._desktop = self._registry.getDesktop(0)
		self._root = self._desktop[0]
		self._rset = self._root.getRelationSet()
		test.assertEqual(len(self._rset), 4, "Num relations expected %d, recieved %d" % (6, len(self._rset)))

	def test_getRelationType(self, test):
		expected = [pyatspi.RELATION_EMBEDS,
			    pyatspi.RELATION_PARENT_WINDOW_OF,
			    pyatspi.RELATION_DESCRIBED_BY,
			    pyatspi.RELATION_POPUP_FOR,]
		rtypes = [rel.getRelationType() for rel in self._rset]
		for exp, type in zip(expected, rtypes):
			test.assertEqual(exp, type, "Relation type expected %s, recieved %s" % (exp, type))

	def test_getRelationTypeName(self, test):
		# FIXME This may not have been implemented in CORBA.
		#       Completely unused?
		pass

	def test_getNTargets(self, test):
		expected = [1, 1, 1, 3]
		ntargs = [rel.getNTargets() for rel in self._rset]
		for exp, ntarg in zip(expected, ntargs):
			test.assertEqual(exp, ntarg, "Number of targets expected %s, recieved %s" % (exp, ntarg))

	def test_getTarget(self, test):
		rone = self._rset[0]
		tone = rone.getTarget(0)
		tonename = tone.name
		test.assertEqual(tonename, "r1", "Target name expected %s, recieved %s" % ("r1", tonename))
		tonerole = tone.getRoleName()
		test.assertEqual(tonerole, "invalid", "Target RoleName expected %s, recieved %s" % ("invalid", tonename))

		rtwo = self._rset[3]
		ttwo = rtwo.getTarget(2)
		ttwoname = ttwo.name
		test.assertEqual(ttwoname, "m3", "Target name expected %s, recieved %s" % ("m3", ttwoname))
		ttworole = ttwo.getRoleName()
		test.assertEqual(ttworole, "invalid", "Target RoleName expected %s, recieved %s" % ("invalid", ttwoname))

	def teardown(self, test):
		pass
