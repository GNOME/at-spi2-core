'''
pyatspi: Python wrapper for AT-SPI

@author: Peter Parente
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
from distutils.core import setup

doclines = __doc__.split('\n')
setup(name='pyatspi',
      version='0.1.0',
      author='Peter Parente',
      author_email='parente@cs.unc.edu',
      url='http://live.gnome.org/GAP/PythonATSPI',
      license='http://www.opensource.org/licenses/lgpl-license.php',
      description = doclines[1],     
      package_dir = {"pyatspi" : ""},
      packages = ['pyatspi'],
      )
