'''
Defines constants used throughout this wrapper.

@author: Peter Parente
@author: Pete Brunet
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
import Accessibility
import utils

# pull dictionary of Accessibility module namespace into local var, temporarily
acc_dict = vars(Accessibility)

# run through all objects in the Accessibility module
ALL_INTERFACES = []
# list of classes that are not queriable interfaces
not_interfaces = ['RoleSet', 'StateSet', 'DeviceEventListener', 'LoginHelper',
                  'ContentStream', 'DeviceEventController', 'Registry'
                  'DeviceEventListener', 'EventListener', 'Relation', 
                  'CommandListener', 'Selector']
for obj in acc_dict.values():
  try:
    # see if the object has a typecode
    kind = obj.__typecode__.kind
  except AttributeError:
    continue
  # compare the typecode to the one for CORBA objects, and to our list of 
  # classes that are not queriable interfaces
  if (kind == ORBit.CORBA.tk_objref and 
      utils.getInterfaceName(obj) not in not_interfaces):
    # this is an interface class
    ALL_INTERFACES.append(obj)
# get rid of our temporary vars
del not_interfaces, obj, kind

# constants used in the Component interface to get screen coordinates
DESKTOP_COORDS = 0
WINDOW_COORDS = 1

# constants used to synthesize mouse events
MOUSE_B1P = 'b1p'
MOUSE_B1R = 'b1r'
MOUSE_B1C = 'b1c'
MOUSE_B1D = 'b1d'
MOUSE_B2P = 'b2p'
MOUSE_B2R = 'b2r'
MOUSE_B2C = 'b2c'
MOUSE_B2D = 'b2d'
MOUSE_B3P = 'b3p'
MOUSE_B3R = 'b3r'
MOUSE_B3C = 'b3c'
MOUSE_B3D = 'b3d'
MOUSE_ABS = 'abs'
MOUSE_REL = 'rel'

# defines levels of caching where if x > y, x caches all of y plus more
CACHE_INTERFACES = 0
CACHE_PROPERTIES = 1

# events that clear cached properties
CACHE_EVENTS = ['object:property-change:accessible-name',
                'object:property-change:accessible-description',
                'object:property-change:accessible-role',
                'object:property-change:accessible-parent']

# dictionary used to correct the bug of not being able to register for all the
# subevents given only an AT-SPI event class (i.e. first part of the event
# name) keys are event names having subevents and values are the subevents
# under the key event; handlers *can* be registered for events not in this tree
EVENT_TREE = {
  'terminal':
    ['terminal:line-changed',
     'terminal:columncount-changed',
     'terminal:linecount-changed',
     'terminal:application-changed',
     'terminal:charwidth-changed'
     ],
  'document':
    ['document:load-complete',
     'document:reload',
     'document:load-stopped',
     'document:content-changed',
     'document:attributes-changed'
     ],
  'object': 
    ['object:property-change',
     'object:bounds-changed',
     'object:link-selected',
     'object:state-changed',
     'object:children-changed',
     'object:visible-data-changed',
     'object:selection-changed',
     'object:model-changed',
     'object:active-descendant-changed',
     'object:row-inserted',
     'object:row-reordered',
     'object:row-deleted',
     'object:column-inserted',
     'object:column-reordered',
     'object:column-deleted',
     'object:text-bounds-changed',
     'object:text-selection-changed',
     'object:text-changed',
     'object:text-attributes-changed',
     'object:text-caret-moved',  
     'object:attributes-changed'],
  'object:text-changed' :
    ['object:text-changed:insert',
    'object:text-changed:delete'],
  'object:property-change' :
    ['object:property-change:accessible-parent', 
    'object:property-change:accessible-name',
    'object:property-change:accessible-description',
    'object:property-change:accessible-value',
    'object:property-change:accessible-role',
    'object:property-change:accessible-table-caption',
    'object:property-change:accessible-table-column-description',
    'object:property-change:accessible-table-column-header',
    'object:property-change:accessible-table-row-description',
    'object:property-change:accessible-table-row-header',
    'object:property-change:accessible-table-summary'],
  'object:children-changed' :
    ['object:children-changed:add',
    'object:children-changed:remove'],
  'object:state-changed' :
    ['object:state-changed:'],
  'mouse' :
    ['mouse:abs',
    'mouse:rel',
    'mouse:button'],
  'mouse:button' :
    ['mouse:button:1p',
    'mouse:button:1r',
    'mouse:button:2p',
    'mouse:button:2r',
    'mouse:button:3p',
    'mouse:button:3r'],
  'window' :
    ['window:minimize',
    'window:maximize',
    'window:restore',
    'window:close',
    'window:create',
    'window:reparent',
    'window:desktop-create',
    'window:desktop-destroy',
    'window:activate',
    'window:deactivate',
    'window:raise',
    'window:lower',
    'window:move',
    'window:resize',
    'window:shade',
    'window:unshade',
    'window:restyle'],
  'focus' :
    ['focus:']
}

# pull ROLE_*, STATE_*, TEXT_*, MODIFIER_*, LOCALE_*, and RELATION_*, etc. 
# constants into the local namespace for convenient access
# grab all the variable names and their values from the Accessibility module

# get the dictionary for the local namespace
loc_dict = locals()
# these are the prefixes for the variable names we want to pull out of the 
# Accessibility module
prefixes = ['ROLE_', 'STATE_', 'TEXT_', 'MODIFIER_', 'LOCALE_', 'RELATION_',
            'KEY_', 'MATCH_', 'SORT_', 'LAYER_']
# for each variable name in the Accessibility namespace, check if it starts
# with at least one of the prefixes above; if it does, add a 2-tuple of 
# variable name and value to the values list
values = ((name, value) for name, value in acc_dict.items()
          if len([p for p in prefixes if name.startswith(p)]))
# create a new dictionary from the list of tuples and then update the local
# namespace dictionary with that dictionary
loc_dict.update(dict(values))

# build a dictionary mapping state values to names based on the prefix of the
# constant name imported from Accessibility
STATE_VALUE_TO_NAME = dict(((value, name[6:].lower().replace('_', ' ')) 
                            for name, value 
                            in acc_dict.items()
                            if name.startswith('STATE_')))

# build a dictionary mapping relation values to names based on the prefix of 
# the constant name imported from Accessibility
RELATION_VALUE_TO_NAME = dict(((value, name[9:].lower().replace('_', ' ')) 
                               for name, value 
                               in acc_dict.items()
                               if name.startswith('RELATION_')))

# throw away any temporary variables so they don't hang around in this module
del acc_dict, loc_dict, prefixes, values
