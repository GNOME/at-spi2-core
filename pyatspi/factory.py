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

from base import *
from accessible import *

ATSPI_ACCESSIBLE = 'org.freedesktop.atspi.Accessible'
ATSPI_ACTION = 'org.freedesktop.atspi.Action'
ATSPI_APPLICATION = 'org.freedesktop.atspi.Application'
ATSPI_COMPONENT = 'org.freedesktop.atspi.Component'
ATSPI_DOCUMENT = 'org.freedesktop.atspi.Document'
ATSPI_EDITABLE_TEXT = 'org.freedesktop.atspi.EditableText'
ATSPI_HYPERLINK = 'org.freedesktop.atspi.Hyperlink'
ATSPI_HYPERTEXT = 'org.freedesktop.atspi.Hypertext'
ATSPI_IMAGE = 'org.freedesktop.atspi.Image'
ATSPI_LOGIN_HELPER = 'org.freedesktop.atspi.LoginHelper'
ATSPI_SELECTION = 'org.freedesktop.atspi.Selection'
ATSPI_SELECTOR = 'org.freedesktop.atspi.Selector'
ATSPI_STREAMABLE_CONTENT = 'org.freedesktop.atspi.Content'
ATSPI_TABLE = 'org.freedesktop.atspi.Table'
ATSPI_TEXT = 'org.freedesktop.atspi.Text'
ATSPI_VALUE = 'org.freedesktop.atspi.Value'

#------------------------------------------------------------------------------

_interfaces = {
		ATSPI_ACCESSIBLE:Accessible,
		#ATSPI_ACTION:
		#ATSPI_APPLICATION:
		#ATSPI_COMPONENT:
		#ATSPI_DOCUMENT:
		#ATSPI_EDITABLE_TEXT:
		#ATSPI_HYPERLINK:
		#ATSPI_HYPERTEXT:
		#ATSPI_IMAGE:
		#ATSPI_LOGIN_HELPER:
		#ATSPI_SELECTION:
		#ATSPI_SELECTOR:
		#ATSPI_STREAMABLE_CONTENT:
		#ATSPI_TABLE:
		#ATSPI_TEXT:
		#ATSPI_TREE:
		#ATSPI_VALUE:
}

def interfaceFactory(self, busobject, cache, app, path, interface):
	"""
	The queryInterfaces method needs to return
	different atspi interfaces depending on the interface name.
	This class registers names and ATSPI interface
	classes to perform this task.
	"""
	return _interfaces[interface](object, cache, app, path, interface)

#END----------------------------------------------------------------------------
