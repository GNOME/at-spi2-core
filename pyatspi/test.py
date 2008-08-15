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

from cache import AccessibleCache
from factory import create_accessible

import constants

#------------------------------------------------------------------------------

class TestApplicationCache(object):
	"""
	Test application cache. Accesses single AccessibleCache.
	"""

	def __init__(self, connection, bus_name):
		self._connection = connection
		self._bus_name = bus_name
		self._accessible_cache = AccessibleCache(connection, bus_name)

	def __getitem__(self, key):
		return self._accessible_cache

	def __contains__(self, key):
		if key == self._bus_name:
			return True
		else:
			return False

	def get_root(self):
		return create_accessible(self,
					 self._bus_name, 
					 self._accessible_cache.root, 
					 None,
					 constants.ATSPI_ACCESSIBLE,
					 connection=self._connection)

	root = property(fget=get_root)
