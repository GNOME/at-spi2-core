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

import interfaces
from base import Enum as _Enum
from factory import create_accessible

#------------------------------------------------------------------------------

class RelationType(_Enum):
    _enum_lookup = {
        0:'RELATION_NULL',
        1:'RELATION_LABEL_FOR',
        2:'RELATION_LABELLED_BY',
        3:'RELATION_CONTROLLER_FOR',
        4:'RELATION_CONTROLLED_BY',
        5:'RELATION_MEMBER_OF',
        6:'RELATION_TOOLTIP_FOR',
        7:'RELATION_NODE_CHILD_OF',
        8:'RELATION_EXTENDED',
        9:'RELATION_FLOWS_TO',
        10:'RELATION_FLOWS_FROM',
        11:'RELATION_SUBWINDOW_OF',
        12:'RELATION_EMBEDS',
        13:'RELATION_EMBEDDED_BY',
        14:'RELATION_POPUP_FOR',
        15:'RELATION_PARENT_WINDOW_OF',
        16:'RELATION_DESCRIPTION_FOR',
        17:'RELATION_DESCRIBED_BY',
        18:'RELATION_LAST_DEFINED',
    }

#------------------------------------------------------------------------------

RELATION_CONTROLLED_BY = RelationType(4)

RELATION_CONTROLLER_FOR = RelationType(3)

RELATION_DESCRIBED_BY = RelationType(17)

RELATION_DESCRIPTION_FOR = RelationType(16)

RELATION_EMBEDDED_BY = RelationType(13)

RELATION_EMBEDS = RelationType(12)

RELATION_EXTENDED = RelationType(8)

RELATION_FLOWS_FROM = RelationType(10)

RELATION_FLOWS_TO = RelationType(9)

RELATION_LABELLED_BY = RelationType(2)

RELATION_LABEL_FOR = RelationType(1)

RELATION_LAST_DEFINED = RelationType(18)

RELATION_MEMBER_OF = RelationType(5)

RELATION_NODE_CHILD_OF = RelationType(7)

RELATION_NULL = RelationType(0)

RELATION_PARENT_WINDOW_OF = RelationType(15)

RELATION_POPUP_FOR = RelationType(14)

RELATION_SUBWINDOW_OF = RelationType(11)

RELATION_TOOLTIP_FOR = RelationType(6)

#------------------------------------------------------------------------------

# Build a dictionary mapping relation values to names based on the prefix of the enum constants.

RELATION_VALUE_TO_NAME = dict(((value, name[9:].lower().replace('_', ' ')) 
                               for name, value 
                               in globals().items()
                               if name.startswith('RELATION_')))

#------------------------------------------------------------------------------

def _marshal_relation_set(cache, app_name, relation_set):
	"""
	The D-Bus protocol has a relation set passed as an array of
	relation types and object arrays.

	This function marshals the D-Bus message into a list of relation
	objects.
	"""
	return [Relation(cache, app_name, *relation) for relation in relation_set]

#------------------------------------------------------------------------------

class Relation(object):
    """
    An interface via which objects' non-hierarchical relationships
    to one another are indicated. An instance of Relations represents
    a "one-to-many" correspondance.
    """

    def __init__(self, cache, app_name, type, objects):
	self._type = type
	self._objects = objects

	self._cache = cache
	self._app_name = app_name
    
    def getNTargets(self):
        """
        @return the number of objects to which this relationship applies.
        """
        return len(self._objects)
    
    def getRelationType(self):
        """
        @return the RelationType of this Relation.
        """
        return self._type
    
    def getRelationTypeName(self):
        """
        @return an unlocalized string representing the relation type.
        """
	return RELATION_VALUE_TO_NAME[self._type]
    
    def getTarget(self, index):
        """
        @return an Object which is the 'nth'target of this Relation,
        e.g. the Object at index i in the list of Objects having the
        specified relationship to this Accessible.
        """
	return create_accessible(self._cache,
			 	 self._app_name,
				 self._objects[index],
				 interfaces.ATSPI_ACCESSIBLE,
				 connection=self._cache._connection)

#END----------------------------------------------------------------------------
