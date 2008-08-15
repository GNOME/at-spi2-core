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

#import registry

#Registry = registry.Registry(reg)
#registry.Registry = Registry
#del registry

def setCacheLevel(level):
	pass

def getCacheLevel():
	return None

def clearCache():
	pass

def printCache():
	print "Print cache function is deprecated";

import other

# Build a dictionary mapping state values to names based on the prefix of the
# enum constants.
STATE_VALUE_TO_NAME = dict(((value, name[6:].lower().replace('_', ' ')) 
                            for name, value 
                            in vars(other).items()
                            if name.startswith('STATE_')))

# Build a dictionary mapping relation values to names based on the prefix of 
# the enum constants.
RELATION_VALUE_TO_NAME = dict(((value, name[9:].lower().replace('_', ' ')) 
                               for name, value 
                               in vars(other).items()
                               if name.startswith('RELATION_')))

del other

from constants import *

from base import *
from other import *

from test import *

#from utils import *
