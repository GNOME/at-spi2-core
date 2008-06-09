'''
Utility functions for AT-SPI for querying interfaces, searching the hierarchy,
converting constants to strings, and so forth.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2005, 2007 IBM Corporation
@license: LGPL

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Portions of this code originally licensed and copyright (c) 2005, 2007
IBM Corporation under the BSD license, available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''
import ORBit
import Accessibility__POA

def getInterfaceIID(obj):
  '''
  Gets the ID of an interface class or object in string format for use in
  queryInterface.
  
  @param obj: Class representing an AT-SPI interface or instance
  @type obj: object
  @return: IID for the interface
  @rtype: string
  @raise AttributeError: When the parameter does not provide typecode info
  '''
  return obj.__typecode__.repo_id

def getInterfaceName(obj):
  '''
  Gets the human readable name of an interface class or object in string
  format.
  
  @param obj: Class representing an AT-SPI interface or instance
  @type obj: class
  @return: Name of the interface
  @rtype: string
  @raise AttributeError: When the parameter does not provide typecode info
  '''
  return obj.__typecode__.name

# we're importing here to avoid cyclic importants; constants relies on the
# two functions above
import constants

def listInterfaces(obj):
  '''
  Gets a list of the names of all interfaces supported by this object. The
  names are the short-hand interface names like "Accessible" and "Component",
  not the full interface identifiers.

  @param obj: Arbitrary object to query for all accessibility related
  interfaces. Must provide a queryInterface method.
  @type obj: object
  @return: Set of supported interface names
  @rtype: set
  @raise AttributeError: If the object provide does not implement
  queryInterface
  '''
  names = set()
  for ic in constants.ALL_INTERFACES:
    io = obj.queryInterface(getInterfaceIID(ic))
    if io is None:
      continue
    names.add(getInterfaceName(ic))
  return names

def stringToConst(prefix, suffix):
  '''
  Maps a string name to an AT-SPI constant. The rules for the mapping are as 
  follows:
    - The prefix is captalized and has an _ appended to it.
    - All spaces in the suffix are mapped to the _ character. 
    - All alpha characters in the suffix are mapped to their uppercase.
    
  The resulting name is used with getattr to look up a constant with that name
  in the L{constants} module. If such a constant does not exist, the string
  suffix is returned instead.

  This method allows strings to be used to refer to roles, relations, etc.
  without direct access to the constants. It also supports the future expansion
  of roles, relations, etc. by allowing arbitrary strings which may or may not
  map to the current standard set of roles, relations, etc., but may still
  match some non-standard role, relation, etc. being reported by an
  application.
  
  @param prefix: Prefix of the constant name such as role, relation, state, 
    text, modifier, key
  @type prefix: string
  @param suffix: Name of the role, relation, etc. to use to lookup the constant
  @type suffix: string
  @return: The matching constant value
  @rtype: object
  '''
  name = prefix.upper()+'_'+suffix.upper().replace(' ', '_')
  return getattr(constants, name, suffix)

def stateToString(value):
  '''
  Converts a state value to a string based on the name of the state constant in
  the L{constants} module that has the given value.
  
  @param value: An AT-SPI state
  @type value: Accessibility.StateType
  @return: Human readable, untranslated name of the state
  @rtype: string
  '''
  return constants.STATE_VALUE_TO_NAME.get(value)

def relationToString(value):
  '''
  Converts a relation value to a string based on the name of the state constant
  in the L{constants} module that has the given value.
  
  @param value: An AT-SPI relation
  @type value: Accessibility.RelationType
  @return: Human readable, untranslated name of the relation
  @rtype: string
  '''
  return constants.RELATION_VALUE_TO_NAME.get(value)

def allModifiers():
  '''
  Generates all possible keyboard modifiers for use with 
  L{registry.Registry.registerKeystrokeListener}.
  '''
  mask = 0
  while mask <= (1 << constants.MODIFIER_NUMLOCK):
    yield mask
    mask += 1

def findDescendant(acc, pred, breadth_first=False):
  '''
  Searches for a descendant node satisfying the given predicate starting at 
  this node. The search is performed in depth-first order by default or
  in breadth first order if breadth_first is True. For example,
  
  my_win = findDescendant(lambda x: x.name == 'My Window')
  
  will search all descendants of x until one is located with the name 'My
  Window' or all nodes are exausted. Calls L{_findDescendantDepth} or
  L{_findDescendantBreadth} to start the recursive search.
  
  @param acc: Root accessible of the search
  @type acc: Accessibility.Accessible
  @param pred: Search predicate returning True if accessible matches the 
      search criteria or False otherwise
  @type pred: callable
  @param breadth_first: Search breadth first (True) or depth first (False)?
  @type breadth_first: boolean
  @return: Accessible matching the criteria or None if not found
  @rtype: Accessibility.Accessible or None
  '''
  if breadth_first:
    return _findDescendantBreadth(acc, pred)

  for child in acc:
    try:
      ret = _findDescendantDepth(acc, pred)
    except Exception:
      ret = None
    if ret is not None: return ret

def _findDescendantBreadth(acc, pred):
  '''    
  Internal function for locating one descendant. Called by L{findDescendant} to
  start the search.
  
  @param acc: Root accessible of the search
  @type acc: Accessibility.Accessible
  @param pred: Search predicate returning True if accessible matches the 
      search criteria or False otherwise
  @type pred: callable
  @return: Matching node or None to keep searching
  @rtype: Accessibility.Accessible or None
  '''
  for child in acc:
    try:
      if pred(child): return child
    except Exception:
      pass
  for child in acc:
    try:
      ret = _findDescendantBreadth(child, pred)
    except Exception:
      ret = None
    if ret is not None: return ret

def _findDescendantDepth(acc, pred):
  '''
  Internal function for locating one descendant. Called by L{findDescendant} to
  start the search.

  @param acc: Root accessible of the search
  @type acc: Accessibility.Accessible
  @param pred: Search predicate returning True if accessible matches the 
    search criteria or False otherwise
  @type pred: callable
  @return: Matching node or None to keep searching
  @rtype: Accessibility.Accessible or None
  '''
  try:
    if pred(acc): return acc
  except Exception:
    pass
  for child in acc:
    try:
      ret = _findDescendantDepth(child, pred)
    except Exception:
      ret = None
    if ret is not None: return ret
    
def findAllDescendants(acc, pred):
  '''
  Searches for all descendant nodes satisfying the given predicate starting at 
  this node. Does an in-order traversal. For example,
  
  pred = lambda x: x.getRole() == pyatspi.ROLE_PUSH_BUTTON
  buttons = pyatspi.findAllDescendants(node, pred)
  
  will locate all push button descendants of node.
  
  @param acc: Root accessible of the search
  @type acc: Accessibility.Accessible
  @param pred: Search predicate returning True if accessible matches the 
      search criteria or False otherwise
  @type pred: callable
  @return: All nodes matching the search criteria
  @rtype: list
  '''
  matches = []
  _findAllDescendants(acc, pred, matches)
  return matches

def _findAllDescendants(acc, pred, matches):
  '''
  Internal method for collecting all descendants. Reuses the same matches
  list so a new one does not need to be built on each recursive step.
  '''
  for child in acc:
    try:
      if pred(child): matches.append(child)
    except Exception:
      pass
    _findAllDescendants(child, pred, matches)
  
def findAncestor(acc, pred):
  '''
  Searches for an ancestor satisfying the given predicate. Note that the
  AT-SPI hierarchy is not always doubly linked. Node A may consider node B its
  child, but B is not guaranteed to have node A as its parent (i.e. its parent
  may be set to None). This means some searches may never make it all the way
  up the hierarchy to the desktop level.
  
  @param acc: Starting accessible object
  @type acc: Accessibility.Accessible
  @param pred: Search predicate returning True if accessible matches the 
    search criteria or False otherwise
  @type pred: callable
  @return: Node matching the criteria or None if not found
  @rtype: Accessibility.Accessible
  '''
  if acc is None:
    # guard against bad start condition
    return None
  while 1:
    if acc.parent is None:
      # stop if there is no parent and we haven't returned yet
      return None
    try:
      if pred(acc.parent): return acc.parent
    except Exception:
      pass
    # move to the parent
    acc = acc.parent

def getPath(acc):
  '''
  Gets the path from the application ancestor to the given accessible in
  terms of its child index at each level.
  
  @param acc: Target accessible
  @type acc: Accessibility.Accessible
  @return: Path to the target
  @rtype: list of integer
  @raise LookupError: When the application accessible cannot be reached
  '''
  path = []
  while 1:
    if acc.parent is None:
      path.reverse()
      return path
    try:
      path.append(acc.getIndexInParent())
    except Exception:
      raise LookupError
    acc = acc.parent

class _StateSetImpl(Accessibility__POA.StateSet):
  '''
  Implementation of the StateSet interface. Clients should not use this class
  directly, but rather the L{StateSet} proxy class.
  
  @param states: Set of states
  @type states: set
  '''
  def __init__(self):
    '''Initializes the state set.'''
    self.states = set()
    
  def contains(self, state):
    '''
    Checks if this StateSet contains the given state.
    
    @param state: State to check
    @type state: Accessibility.StateType
    @return: True if the set contains the given state
    @rtype: boolean
    '''
    return state in self.states
  
  def add(self, state):
    '''
    Adds a state to this set.
    
    @param state: State to add
    @type state: Accessibility.StateType
    '''
    self.states.add(state)
  
  def remove(self, state):
    '''
    Removes a state from this set.
    
    @param state: State to remove
    @type state: Accessibility.StateType
    '''
    self.states.remove(state)
  
  def equals(self, state_set):
    '''
    Checks if this StateSet contains exactly the same members as the given
    StateSet.
    
    @param state_set: Another set
    @type state_set: Accessibility.StateSet
    @return: Are the sets equivalent in terms of their contents?
    @rtype: boolean
    '''
    # don't check private members, object might be from another process
    # or implementation
    return set(state_set.getStates()) == self.states
  
  def compare(self, state_set):
    '''
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
    '''
    raise ORBit.CORBA.NO_IMPLEMENT
    
    # don't check private members, object might be from another process
    # or implementation
    #states = set(state_set.getStates())
    #diff = self.states.symmetric_difference(states)
    #new_ss = _StateSetImpl()
    #map(new_ss._this().add, diff)
    #return new_ss._this()
  
  def isEmpty(self):
    '''
    Checks if this L{StateSet} is empty.
    
    @return: Is it empty?
    @rtype: boolean
    '''
    return len(self.states) == 0

  def getStates(self):
    '''
    Gets the sequence of all states in this set.
    
    @return: List of states
    @rtype: list
    '''
    return list(self.states)

class StateSet(object):
  '''
  Python proxy for the L{_StateSetImpl} class. Use this to safely instantiate
  new StateSet objects in Python.

  @param impl: State set implementation
  @type impl: L{_StateSetImpl}
  '''
  def __init__(self, *states):
    '''
    Initializes the state set with the given states.

    @param states: States to add immediately
    @type states: list
    '''
    self.impl = _StateSetImpl()
    map(self.impl._this().add, states)
    
  def contains(self, state):
    '''
    Checks if this StateSet contains the given state.
    
    @param state: State to check
    @type state: Accessibility.StateType
    @return: True if the set contains the given state
    @rtype: boolean
    '''
    return self.impl._this().contains(state)
  
  def add(self, *states):
    '''
    Adds states to this set.
    
    @param states: State(s) to add
    @type states: Accessibility.StateType
    '''
    map(self.impl._this().add, states)
    
  def remove(self, state):
    '''
    Removes states from this set.
    
    @param states: State(s) to remove
    @type states: Accessibility.StateType
    '''
    map(self.impl._this().remove, state)
  
  def equals(self, state_set):
    '''
    Checks if this StateSet contains exactly the same members as the given
    StateSet.
    
    @param state_set: Another set
    @type state_set: Accessibility.StateSet
    @return: Are the sets equivalent in terms of their contents?
    @rtype: boolean
    '''
    if isinstance(state_set, self.__class__):
      # convenience if we're given a proxy
      state_set = state_set.raw()
    return self.impl._this().equals(state_set)
  
  def compare(self, state_set):
    '''
    Finds the symmetric difference between this state set andthe one provided,
    and returns it as a new StateSet.

    @note: This does not use L{_StateSetImpl.compare} which cannot be
    implemented at this time
    @param state_set: Set to compare against
    @type state_set: Accessibility.StateSet
    @return: Proxy for the new set
    @rtype: L{StateSet}
    '''
    if isinstance(state_set, self.__class__):
      # shortcut if it's another one of our proxies
      state_set = state_set.raw()
    a = set(self.impl._this().getStates())
    b = set(state_set.getStates())
    diff = a.symmetric_difference(b)
    return StateSet(*diff)
  
  def isEmpty(self):
    '''
    Checks if this StateSet is empty.
    
    @return: Is it empty?
    @rtype: boolean
    '''
    return self.impl._this().isEmpty()

  def getStates(self):
    '''
    Gets the sequence of all states in this set.
    
    @return: List of states
    @rtype: list
    '''
    return self.impl._this().getStates()

  def raw(self):
    '''
    Gets the Accessibility.StateSet object proxied for use in a remote
    call.

    @return: State set
    @rtype: Accessibility.StateSet
    '''
    return self.impl._this()
