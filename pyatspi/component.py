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
from factory import accessible_factory
from accessible import BoundingBox, Accessible

from dbus.types import Int16

__all__ = [
           "CoordType",
           "XY_SCREEN",
           "XY_WINDOW",
           "ComponentLayer",
           "Component",
           "LAYER_BACKGROUND",
           "LAYER_CANVAS",
           "LAYER_INVALID",
           "LAYER_LAST_DEFINED",
           "LAYER_MDI",
           "LAYER_OVERLAY",
           "LAYER_POPUP",
           "LAYER_WIDGET",
           "LAYER_WINDOW",
          ]

#------------------------------------------------------------------------------

class CoordType(Enum):
        _enum_lookup = {
                0:'XY_SCREEN',
                1:'XY_WINDOW',
        }

XY_SCREEN = CoordType(0)
XY_WINDOW = CoordType(1)

#------------------------------------------------------------------------------

class ComponentLayer(Enum):
        _enum_lookup = {
                0:'LAYER_INVALID',
                1:'LAYER_BACKGROUND',
                2:'LAYER_CANVAS',
                3:'LAYER_WIDGET',
                4:'LAYER_MDI',
                5:'LAYER_POPUP',
                6:'LAYER_OVERLAY',
                7:'LAYER_WINDOW',
                8:'LAYER_LAST_DEFINED',
        }

LAYER_BACKGROUND = ComponentLayer(1)
LAYER_CANVAS = ComponentLayer(2)
LAYER_INVALID = ComponentLayer(0)
LAYER_LAST_DEFINED = ComponentLayer(8)
LAYER_MDI = ComponentLayer(4)
LAYER_OVERLAY = ComponentLayer(6)
LAYER_POPUP = ComponentLayer(5)
LAYER_WIDGET = ComponentLayer(3)
LAYER_WINDOW = ComponentLayer(7)

#------------------------------------------------------------------------------

class Component(Accessible):
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
                func = self.get_dbus_method("contains", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

        def deregisterFocusHandler(self, *args, **kwargs):
                """
                Request that an EventListener registered via registerFocusHandler
                no longer be notified when this object receives keyboard focus.
                """
                func = self.get_dbus_method("deregisterFocusHandler", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

        def getAccessibleAtPoint(self, *args, **kwargs):
                """
                @return the Accessible child whose bounding box contains the
                specified point.
                """
                #TODO this needs a real implementation
                func = self.get_dbus_method("getAccessibleAtPoint", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

        def getAlpha(self, *args, **kwargs):
                """
                Obtain the alpha value of the component. An alpha value of 1.0
                or greater indicates that the object is fully opaque, and an
                alpha value of 0.0 indicates that the object is fully transparent.
                Negative alpha values have no defined meaning at this time.
                """
                func = self.get_dbus_method("getAlpha", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

        def getExtents(self, coord_type):
                """
                Obtain the Component's bounding box, in pixels, relative to the
                specified coordinate system. 
                @param coord_type
                @return a BoundingBox which entirely contains the object's onscreen
                visual representation.
                """
                func = self.get_dbus_method("getExtents", dbus_interface=ATSPI_COMPONENT)
                extents = func(Int16(coord_type))
                return BoundingBox(*extents)

        def getLayer(self, *args, **kwargs):
                """
                @return the ComponentLayer in which this object resides.
                """
                func = self.get_dbus_method("getLayer", dbus_interface=ATSPI_COMPONENT)
                return ComponentLayer(func(*args, **kwargs))

        def getMDIZOrder(self):
                """
                Obtain the relative stacking order (i.e. 'Z' order) of an object.
                Larger values indicate that an object is on "top" of the stack,
                therefore objects with smaller MDIZOrder may be obscured by objects
                with a larger MDIZOrder, but not vice-versa. 
                @return an integer indicating the object's place in the stacking
                order.
                """
                func = self.get_dbus_method("getMDIZOrder", dbus_interface=ATSPI_COMPONENT)
                return func()

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
                func = self.get_dbus_method("getPosition", dbus_interface=ATSPI_COMPONENT)
                return func(Int16(coord_type))

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
                func = self.get_dbus_method("getSize", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

        def grabFocus(self, *args, **kwargs):
                """
                Request that the object obtain keyboard focus.
                @return True if keyboard focus was successfully transferred to
                the Component.
                """
                func = self.get_dbus_method("grabFocus", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

        def registerFocusHandler(self, *args, **kwargs):
                """
                Register an EventListener for notification when this object receives
                keyboard focus.
                """
                func = self.get_dbus_method("registerFocusHandler", dbus_interface=ATSPI_COMPONENT)
                return func(*args, **kwargs)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_COMPONENT, Component)

#END----------------------------------------------------------------------------
