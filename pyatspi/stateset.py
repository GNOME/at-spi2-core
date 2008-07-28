#Copyright (C) 2008 Codethink Ltd
#copyright: Copyright (c) 2005, 2007 IBM Corporation

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU Lesser General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#Portions of this code originally licensed and copyright (c) 2005, 2007
#IBM Corporation under the BSD license, available at
#U{http://www.opensource.org/licenses/bsd-license.php}

#authors: Peter Parente, Mark Doffman

class _StateSetImpl(Accessibility__POA.StateSet):
	"""
	Implementation of the StateSet interface. Clients should not use this class
	directly, but rather the L{StateSet} proxy class.
	
	@param states: Set of states
	@type states: set
	"""
	def __init__(self):
		"""Initializes the state set."""
		self.states = set()
		
	def contains(self, state):
		"""
		Checks if this StateSet contains the given state.
		
		@param state: State to check
		@type state: Accessibility.StateType
		@return: True if the set contains the given state
		@rtype: boolean
		"""
		return state in self.states
	
	def add(self, state):
		"""
		Adds a state to this set.
		
		@param state: State to add
		@type state: Accessibility.StateType
		"""
		self.states.add(state)
	
	def remove(self, state):
		"""
		Removes a state from this set.
		
		@param state: State to remove
		@type state: Accessibility.StateType
		"""
		self.states.remove(state)
	
	def equals(self, state_set):
		"""
		Checks if this StateSet contains exactly the same members as the given
		StateSet.
		
		@param state_set: Another set
		@type state_set: Accessibility.StateSet
		@return: Are the sets equivalent in terms of their contents?
		@rtype: boolean
		"""
		# don't check private members, object might be from another process
		# or implementation
		return set(state_set.getStates()) == self.states
	
	def compare(self, state_set):
		"""
		Computes the symmetric differences of this L{StateSet} and the given
		L{StateSet}.

		@note: This method is not currently implemented because of difficulties
		with reference counting. This method needs to return a new
		Accessibility.StateSet object, but the Python implementation for that
		object needs to be kept alive. The problem is who will keep that
		server implementation alive? As soon as it goes out of scope, it's
		GC'ed. This object cannot keep it alive either as it may go out of
		scope before the new object is ready to be finalized. With a global
		cache of objects, we don't know when to invalidate.
		
		@param state_set: Another set
		@type state_set: Accessibility.StateSet
		@return: Elements in only one of the two sets
		@rtype: Accessibility.StateSet
		"""
		raise ORBit.CORBA.NO_IMPLEMENT
		
		# don't check private members, object might be from another process
		# or implementation
		#states = set(state_set.getStates())
		#diff = self.states.symmetric_difference(states)
		#new_ss = _StateSetImpl()
		#map(new_ss._this().add, diff)
		#return new_ss._this()
	
	def isEmpty(self):
		"""
		Checks if this L{StateSet} is empty.
		
		@return: Is it empty?
		@rtype: boolean
		"""
		return len(self.states) == 0

	def getStates(self):
		"""
		Gets the sequence of all states in this set.
		
		@return: List of states
		@rtype: list
		"""
		return list(self.states)

class StateSet(object):
	"""
	Python proxy for the L{_StateSetImpl} class. Use this to safely instantiate
	new StateSet objects in Python.

	@param impl: State set implementation
	@type impl: L{_StateSetImpl}
	"""
	def __init__(self, *states):
		"""
		Initializes the state set with the given states.

		@param states: States to add immediately
		@type states: list
		"""
		self.impl = _StateSetImpl()
		map(self.impl._this().add, states)
		
	def contains(self, state):
		"""
		Checks if this StateSet contains the given state.
		
		@param state: State to check
		@type state: Accessibility.StateType
		@return: True if the set contains the given state
		@rtype: boolean
		"""
		return self.impl._this().contains(state)
	
	def add(self, *states):
		"""
		Adds states to this set.
		
		@param states: State(s) to add
		@type states: Accessibility.StateType
		"""
		map(self.impl._this().add, states)
		
	def remove(self, state):
		"""
		Removes states from this set.
		
		@param states: State(s) to remove
		@type states: Accessibility.StateType
		"""
		map(self.impl._this().remove, state)
	
	def equals(self, state_set):
		"""
		Checks if this StateSet contains exactly the same members as the given
		StateSet.
		
		@param state_set: Another set
		@type state_set: Accessibility.StateSet
		@return: Are the sets equivalent in terms of their contents?
		@rtype: boolean
		"""
		if isinstance(state_set, self.__class__):
			# convenience if we're given a proxy
			state_set = state_set.raw()
		return self.impl._this().equals(state_set)
	
	def compare(self, state_set):
		"""
		Finds the symmetric difference between this state set andthe one provided,
		and returns it as a new StateSet.

		@note: This does not use L{_StateSetImpl.compare} which cannot be
		implemented at this time
		@param state_set: Set to compare against
		@type state_set: Accessibility.StateSet
		@return: Proxy for the new set
		@rtype: L{StateSet}
		"""
		if isinstance(state_set, self.__class__):
			# shortcut if it's another one of our proxies
			state_set = state_set.raw()
		a = set(self.impl._this().getStates())
		b = set(state_set.getStates())
		diff = a.symmetric_difference(b)
		return StateSet(*diff)
	
	def isEmpty(self):
		"""
		Checks if this StateSet is empty.
		
		@return: Is it empty?
		@rtype: boolean
		"""
		return self.impl._this().isEmpty()

	def getStates(self):
		"""
		Gets the sequence of all states in this set.
		
		@return: List of states
		@rtype: list
		"""
		return self.impl._this().getStates()

	def raw(self):
		"""
		Gets the Accessibility.StateSet object proxied for use in a remote
		call.

		@return: State set
		@rtype: Accessibility.StateSet
		"""
		return self.impl._this()
