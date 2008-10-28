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

#------------------------------------------------------------------------------

class AccessibleFactory(object):
        __accessible_interfaces = {}

        def register_accessible_class(self, name, cls):
                self.__accessible_interfaces[name] = cls

        def get_accessible_class(self, name):
                return self.__accessible_interfaces[name]

accessible_factory = AccessibleFactory()

#END----------------------------------------------------------------------------
