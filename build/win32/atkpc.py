#!/usr/bin/python
#
# Utility script to generate .pc files for ATK
# for Visual Studio builds, to be used for
# building introspection files

# Author: Fan, Chun-wei
# Date: April 20, 2016

import os
import sys

from replace import replace_multi
from pc_base import BasePCItems

def main(argv):
    base_pc = BasePCItems()
    
    base_pc.setup(argv)
    pkg_replace_items = {'@GLIB_PACKAGES@': 'gobject-2.0 glib-2.0',
                         '@ATK_API_VERSION@': '1.0'}

    pkg_replace_items.update(base_pc.base_replace_items)

    # Generate atk.pc
    replace_multi(base_pc.top_srcdir + '/atk.pc.in',
                  base_pc.srcdir + '/atk.pc',
                  pkg_replace_items)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
