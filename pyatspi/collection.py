#Copyright (C) 2008 Codethink Ltd

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

from interfaces import *
from base import Enum
from accessible import Accessible
from factory import accessible_factory

__all__ = [
            "Collection",
          ]

#------------------------------------------------------------------------------

class Collection(Accessible):

        def createMatchRule(self, *args, **kwargs):
                func = self.get_dbus_method("createMatchRule", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        def freeMatchRule(self, *args, **kwargs):
                func = self.get_dbus_method("freeMatchRule", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        def getActiveDescendant(self, *args, **kwargs):
                func = self.get_dbus_method("getActiveDescendant", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        def getMatches(self, *args, **kwargs):
                func = self.get_dbus_method("getMatches", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        def getMatchesFrom(self, *args, **kwargs):
                func = self.get_dbus_method("getMatchesFrom", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        def getMatchesTo(self, *args, **kwargs):
                func = self.get_dbus_method("getMatchesTo", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        def isAncestorOf(self, *args, **kwargs):
                func = self.get_dbus_method("isAncestorOf", dbus_interface=ATSPI_COLLECTION)
                return func(*args, **kwargs)

        class MatchType(Enum):
                _enum_lookup = {
                        0:'MATCH_INVALID',
                        1:'MATCH_ALL',
                        2:'MATCH_ANY',
                        3:'MATCH_NONE',
                        4:'MATCH_EMPTY',
                        5:'MATCH_LAST_DEFINED',
                }

        MATCH_ALL = MatchType(1)
        MATCH_ANY = MatchType(2)
        MATCH_EMPTY = MatchType(4)
        MATCH_INVALID = MatchType(0)
        MATCH_LAST_DEFINED = MatchType(5)
        MATCH_NONE = MatchType(3)

        class SortOrder(Enum):
                _enum_lookup = {
                        0:'SORT_ORDER_INVALID',
                        1:'SORT_ORDER_CANONICAL',
                        2:'SORT_ORDER_FLOW',
                        3:'SORT_ORDER_TAB',
                        4:'SORT_ORDER_REVERSE_CANONICAL',
                        5:'SORT_ORDER_REVERSE_FLOW',
                        6:'SORT_ORDER_REVERSE_TAB',
                        7:'SORT_ORDER_LAST_DEFINED',
                }

        SORT_ORDER_CANONICAL = SortOrder(1)
        SORT_ORDER_FLOW = SortOrder(2)
        SORT_ORDER_INVALID = SortOrder(0)
        SORT_ORDER_LAST_DEFINED = SortOrder(7)
        SORT_ORDER_REVERSE_CANONICAL = SortOrder(4)
        SORT_ORDER_REVERSE_FLOW = SortOrder(5)
        SORT_ORDER_REVERSE_TAB = SortOrder(6)
        SORT_ORDER_TAB = SortOrder(3)

        class TreeTraversalType(Enum):
                _enum_lookup = {
                        0:'TREE_RESTRICT_CHILDREN',
                        1:'TREE_RESTRICT_SIBLING',
                        2:'TREE_INORDER',
                        3:'TREE_LAST_DEFINED',
                }

        TREE_INORDER = TreeTraversalType(2)
        TREE_LAST_DEFINED = TreeTraversalType(3)
        TREE_RESTRICT_CHILDREN = TreeTraversalType(0)
        TREE_RESTRICT_SIBLING = TreeTraversalType(1)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_COLLECTION, Collection)

#END----------------------------------------------------------------------------
