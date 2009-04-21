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

import dbus
from interfaces import *
from accessible import Accessible
from factory import accessible_factory
from accessible import BoundingBox

__all__ = [
           "Image",
          ]

#------------------------------------------------------------------------------

class Image(Accessible):
        """
        An interface implemented by objects which render image data or
        pictorial information to the screen. When onscreen components
        include graphical information that is not purely intended to
        enhance "3d effect" or visual layout, but which conveys some
        semantic or informational content to the sighted user, they should
        implement Image, and that semantic content should be conveyed
        textually to the extent possible via the image description, as
        well as the Accessible::name and Accessible::description properties.
        """

        def getImageExtents(self, coordType):
                """
                Obtain a bounding box which entirely contains the image contents,
                as displayed on screen. The bounds returned do not account for
                any viewport clipping or the fact that the image may be partially
                or wholly obscured by other onscreen content. 
                @param : coordType
                If 0, the returned bounding box position is returned relative
                to the screen; if 1, the bounding box position is returned relative
                to the containing window. 
                @return a BoundingBox enclosing the image's onscreen representation.
                """
                func = self.get_dbus_method("getImageExtents", dbus_interface=ATSPI_IMAGE)
                return BoundingBox(*func(dbus.Int16(coordType)))

        def getImagePosition(self, coord_type):
                """
                Get the coordinates of the current image position on screen.
                @param : x
                Back-filled with the x coordinate of the onscreen image (i.e.
                the minimum x coordinate) 
                @param : y
                Back-filled with the y coordinate of the onscreen image (i.e.
                the minimum y coordinate) 
                @param : coordType
                If 0, the returned x and y coordinates are returned relative
                to the screen; if 1, they are returned relative to the containing
                window.
                """
                func = self.get_dbus_method("getImagePosition", dbus_interface=ATSPI_IMAGE)
                return func(coord_type)

        def getImageSize(self):
                """
                Obtain the width and height of the current onscreen view of the
                image. The extents returned do not account for any viewport clipping
                or the fact that the image may be partially or wholly obscured
                by other onscreen content. 
                @param : width
                Back-filled with the x extents of the onscreen image (i.e. the
                image width in pixels) 
                @param : height
                Back-filled with the y extents of the onscreen image (i.e. the
                image height in pixels)
                """
                func = self.get_dbus_method("getImageSize", dbus_interface=ATSPI_IMAGE)
                return func()

        def get_imageDescription(self):
                return dbus.String(self._pgetter(self._dbus_interface, "imageDescription"))
        _imageDescriptionDoc = \
                """
                A UTF-8 string providing a textual description of what is visually
                depicted in the image.
                """
        imageDescription = property(fget=get_imageDescription, doc=_imageDescriptionDoc)

        def get_imageLocale(self):
                return dbus.String(self._pgetter(self._dbus_interface, "imageLocale"))
        _imageLocaleDoc = \
                """
                A string corresponding to the POSIX LC_MESSAGES locale used by
                the imageDescription.
                """
        imageLocale = property(fget=get_imageLocale, doc=_imageLocaleDoc)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_IMAGE, Image)

#END----------------------------------------------------------------------------
