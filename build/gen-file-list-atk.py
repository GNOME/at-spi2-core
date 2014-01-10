#!/usr/bin/python
# vim: encoding=utf-8
# Generate the file lists for processing with g-ir-scanner
import os
import sys
import re
import string
import subprocess
import optparse

from msvcfiles import read_vars_from_AM

def gen_atk_filelist(srcroot, subdir, dest):
    vars = read_vars_from_AM(os.path.join(srcroot, subdir, 'Makefile.am'),
                             vars = {'top_builddir':'.'},
                             conds = {'HAVE_INTROSPECTION':True},
                             filters = ['introspection_sources', 'introspection_generated_sources'])

    files = vars['introspection_sources'].split() + \
            vars['introspection_generated_sources'].split()

    with open(dest, 'w') as d:
        for i in files:
            if (i.startswith('./atk/')):
                i = i.replace('./atk/','')
            d.write(srcroot + '\\' + subdir + '\\' + i.replace('/', '\\') + '\n')

def main(argv):
    srcroot = os.path.join('..')
    subdir = 'atk'
    gen_atk_filelist(srcroot, subdir, 'atk_list')
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
