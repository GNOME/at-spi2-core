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
from base import BaseProxyMeta
from accessible import BoundingBox
from state import StateSet

from role import ROLE_UNKNOWN
from component import LAYER_WIDGET

__all__ = [
           "Desktop",
           "DESKTOP_PATH",
          ]

#------------------------------------------------------------------------------

DESKTOP_PATH = '/org/freedesktop/atspi/accessible/desktop'

#------------------------------------------------------------------------------

class DesktopComponent(object):
        """
        The Component interface is implemented by objects which occupy
        on-screen space, e.g. objects which have onscreen visual representations.
        The methods in Component allow clients to identify where the
        objects lie in the onscreen coordinate system, their relative
        size, stacking order, and position. It also provides a mechanism
        whereby keyboard focus may be transferred to specific user interface
        elements programmatically. This is a 2D API, coordinates of 3D
        objects are projected into the 2-dimensional screen view for
        purposes of this interface.
        """

        def contains(self, *args, **kwargs):
                """
                @return True if the specified point lies within the Component's
                bounding box, False otherwise.
                """
                return False

        def getAccessibleAtPoint(self, *args, **kwargs):
                """
                @return the Accessible child whose bounding box contains the
                specified point.
                """
                return None

        def getAlpha(self, *args, **kwargs):
                """
                Obtain the alpha value of the component. An alpha value of 1.0
                or greater indicates that the object is fully opaque, and an
                alpha value of 0.0 indicates that the object is fully transparent.
                Negative alpha values have no defined meaning at this time.
                """
                return 1.0

        def getExtents(self, coord_type):
                """
                Obtain the Component's bounding box, in pixels, relative to the
                specified coordinate system. 
                @param coord_type
                @return a BoundingBox which entirely contains the object's onscreen
                visual representation.
                """
                #TODO This needs to return the window size
                return BoundingBox(*(0,0,1024,768))

        def getLayer(self, *args, **kwargs):
                """
                @return the ComponentLayer in which this object resides.
                """
                return LAYER_WIDGET

        def getMDIZOrder(self):
                """
                Obtain the relative stacking order (i.e. 'Z' order) of an object.
                Larger values indicate that an object is on "top" of the stack,
                therefore objects with smaller MDIZOrder may be obscured by objects
                with a larger MDIZOrder, but not vice-versa. 
                @return an integer indicating the object's place in the stacking
                order.
                """
                return 0

        def getPosition(self, coord_type):
                """
                Obtain the position of the current component in the coordinate
                system specified by coord_type. 
                @param : coord_type
                @param : x
                an out parameter which will be back-filled with the returned
                x coordinate. 
                @param : y
                an out parameter which will be back-filled with the returned
                y coordinate.
                """
                return (0,0)

        def getSize(self, *args, **kwargs):
                """
                Obtain the size, in the coordinate system specified by coord_type,
                of the rectangular area which fully contains the object's visual
                representation, without accounting for viewport clipping. 
                @param : width
                the object's horizontal extents in the specified coordinate system.
                @param : height
                the object's vertical extents in the specified coordinate system.
                """
                #TODO Need to return window size
                return (1024, 768)

        def grabFocus(self, *args, **kwargs):
                """
                Request that the object obtain keyboard focus.
                @return True if keyboard focus was successfully transferred to
                the Component.
                """
                return False

#------------------------------------------------------------------------------

class Desktop(object):
        """
        The base interface which is implemented by all accessible objects.
        All objects support interfaces for querying their contained
        'children' and position in the accessible-object hierarchy,
        whether or not they actually have children.
        """

        __metaclass__ = BaseProxyMeta

        def __init__(self, cache):
                """
                Creates a desktop object. There should be one single desktop
                object for the Registry object.

                @param cache - The application cache.
                @kwarf application - The application D-Bus name

                If the application name is provided the Desktop is being used for
                test and will only report the application provided as its single child.
                """
                self._appcache = cache
                self._app_name = ':'
                self._acc_path = DESKTOP_PATH

        def __str__(self):
                    try:
                              return '[%s | %s]' % (self.getRoleName(), self.name)
                    except Exception:
                              return '[DEAD]'

        def __nonzero__(self):
                        return True

        def __len__(self):
                        return self.getChildCount()

        def __getitem__(self, index):
                        # IndexError thrown by getChildAtIndex
                        return self.getChildAtIndex(index)

        def __eq__(self, other):
                if other is None:
                        return False
                try:
                        if self._app_name == other._app_name and \
                           self._acc_path == other._acc_path:
                                return True
                        else:
                                return False
                except AttributeError:
                        return False

        def __ne__(self, other):
                return not self.__eq__(other)

        def __hash__(self):
                return hash(self._app_name + self._acc_path)

        def getApplication(self):
                """
                Get the containing Application for this object.
                @return the Application instance to which this object belongs.
                """
                return None

        def getAttributes(self):
                """
                Get a list of properties applied to this object as a whole, as
                an AttributeSet consisting of name-value pairs. As such these
                attributes may be considered weakly-typed properties or annotations,
                as distinct from the strongly-typed interface instance data declared
                using the IDL "attribute" keyword.
                Not all objects have explicit "name-value pair" AttributeSet
                properties.
                Attribute names and values may have any UTF-8 string value, however
                where possible, in order to facilitate consistent use and exposure
                of "attribute" properties by applications and AT clients, attribute
                names and values should chosen from a publicly-specified namespace
                where appropriate.
                Where possible, the names and values in the name-value pairs
                should be chosen from well-established attribute namespaces using
                standard semantics. For example, attributes of Accessible objects
                corresponding to XHTML content elements should correspond to
                attribute names and values specified in the w3c XHTML specification,
                at http://www.w3.org/TR/xhtml2, where such values are not already
                exposed via a more strongly-typed aspect of the AT-SPI API. Metadata
                names and values should be chosen from the 'Dublin Core' Metadata
                namespace using Dublin Core semantics: http://dublincore.org/dcregistry/
                Similarly, relevant structural metadata should be exposed using
                attribute names and values chosen from the CSS2 and WICD specification:
                http://www.w3.org/TR/1998/REC-CSS2-19980512 WICD (http://www.w3.org/TR/2005/WD-WICD-20051121/).

                @return : An AttributeSet encapsulating any "attribute values"
                currently defined for the object. An attribute set is a list of strings
                with each string comprising an name-value pair format 'name:value'.
                """
                return []

        def getChildAtIndex(self, index):
                """
                Get the accessible child of this object at index. 
                @param : index
                an in parameter indicating which child is requested (zero-indexed).
                @return : the 'nth' Accessible child of this object.
                """
                return self._appcache.create_application(self._appcache.application_list[index])

        def getIndexInParent(self):
                """
                Get the index of this object in its parent's child list. 
                @return : a long integer indicating this object's index in the
                parent's list.
                """
                return -1

        def getLocalizedRoleName(self):
                """
                Get a string indicating the type of UI role played by this object,
                translated to the current locale.
                @return : a UTF-8 string indicating the type of UI role played
                by this object.
                """
                #TODO Need to localize this somehow. Hmmmmm
                return 'unknown'

        def getRelationSet(self):
                """
                Get a set defining this object's relationship to other accessible
                objects. 
                @return : a RelationSet defining this object's relationships.
                """
                return []

        def getRole(self):
                """
                Get the Role indicating the type of UI role played by this object.
                @return : a Role indicating the type of UI role played by this
                object.
                """
                return ROLE_UNKNOWN

        def getRoleName(self):
                """
                Get a string indicating the type of UI role played by this object.
                @return : a UTF-8 string indicating the type of UI role played
                by this object.
                """
                return 'unknown'

        def getState(self):
                """
                Get the current state of the object as a StateSet. 
                @return : a StateSet encapsulating the currently true states
                of the object.
                """
                return StateSet()

        def isEqual(self, accessible):
                """
                Determine whether an Accessible refers to the same object as
                another. This method should be used rather than brute-force comparison
                of object references (i.e. "by-value" comparison), as two object
                references may have different apparent values yet refer to the
                same object.
                @param : obj
                an Accessible object reference to compare to 
                @return : a boolean indicating whether the two object references
                point to the same object.
                """
                #TODO Fix this method
                return self == accessible

        def get_childCount(self):
                return len(self._appcache.application_list)
        _childCountDoc = \
                """
                childCount: the number of children contained by this object.
                """
        childCount = property(fget=get_childCount, doc=_childCountDoc)

        getChildCount = get_childCount

        def get_description(self):
                return ''
        _descriptionDoc = \
                """
                a string describing the object in more detail than name.
                """
        description = property(fget=get_description, doc=_descriptionDoc)

        def get_name(self):
                return 'main'
        _nameDoc = \
                """
                a (short) string representing the object's name.
                """
        name = property(fget=get_name, doc=_nameDoc)

        def get_parent(self):
                return None
        _parentDoc = \
                """
                An Accessible object which is this object's containing object.
                """
        parent = property(fget=get_parent, doc=_parentDoc)

        @property
        def interfaces(self):
                return [ATSPI_ACCESSIBLE, ATSPI_COMPONENT]

        def queryInterface(self, interface):
                """
                Gets a different accessible interface for this object
                or raises a NotImplemented error if the given interface
                is not supported.
                """
                if interface == ATSPI_ACCESSIBLE:
                                return self
                elif interface == ATSPI_COMPONENT:
                                return DesktopComponent()
                else:
                                raise NotImplementedError(
                                                "%s not supported by accessible object at path %s"
                                                % (interface, self._acc_path))

        def flushCache(self):
                pass

#END----------------------------------------------------------------------------
