import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi
from pyatspi import StateSet

os = [pyatspi.STATE_MULTISELECTABLE,
      pyatspi.STATE_PRESSED,
      pyatspi.STATE_SHOWING,
      pyatspi.STATE_TRANSIENT,
      pyatspi.STATE_COLLAPSED,
      pyatspi.STATE_EDITABLE,]

class StateTest(_PasyTest):

	__tests__ = ["setup",
		     "test_contains",
		     "test_add",
		     "test_remove",
		     "test_equals",
		     "test_compare",
		     "test_isEmpty",
		     "test_getStates",
		     "teardown",
		     ]

	def __init__(self, bus, path):
		_PasyTest.__init__(self, "State", False)

	def setup(self, test):
		pass

	def test_contains(self, test):
		state = StateSet(*os)
		if not state.contains(pyatspi.STATE_PRESSED):
			test.fail("Does not find contained state")
		if state.contains(pyatspi.STATE_ACTIVE):
			test.fail("Finds state not contained")

	def test_add(self, test):
		state = StateSet()
		state.add(pyatspi.STATE_PRESSED)
		if not state.contains(pyatspi.STATE_PRESSED):
			test.fail("State not added")

	def test_remove(self, test):
		state = StateSet(*os)
		state.remove(pyatspi.STATE_PRESSED)
		if state.contains(pyatspi.STATE_PRESSED):
			test.fail("State not removed")

	def test_equals(self, test):
		one = StateSet(*os)
		two = StateSet(*os)
		if not one.equals(two):
			test.fail("Same states not found equal")
		two.remove(pyatspi.STATE_PRESSED)
		if two.equals(one):
			test.fail("Unequal states found equal")

	def test_isEmpty(self, test):
		emp = StateSet()
		if not emp.isEmpty():
			test.fail("Empty state found non-empty")
		emp.add(pyatspi.STATE_PRESSED)
		if emp.isEmpty():
			test.fail("State incorrectly found empty")

	def test_compare(self, test):
		one = StateSet(*os)
		two = StateSet(*os)

		onemtwo = one.compare(two)
		if not onemtwo.isEmpty():
			test.fail("Equal states when compared yeilds non-empty state")

		one.add(pyatspi.STATE_ACTIVE)
		onemtwo = one.compare(two)
		
		act = StateSet(pyatspi.STATE_ACTIVE)
		if not onemtwo.equals(act):
			test.fail("Compared states do not yeild correct state")

	def test_getStates(self, test):
		state = StateSet(*os)

		states = state.getStates()
		cone = set(states)
		ctwo = set(os)

		if not (cone.issubset(ctwo) and ctwo.issubset(cone)):
			test.fail("States not reported correctly")

	def teardown(self, test):
		pass
