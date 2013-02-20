#!/usr/bin/python
# vim: encoding=utf-8
# Generate the file lists for processing with g-ir-scanner
import os
import sys
import re
import string
import subprocess
import optparse

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

def read_vars_from_AM(path, vars = {}, conds = {}, filters = None):
    '''
    path: path to the Makefile.am
    vars: predefined variables
    conds: condition variables for Makefile
    filters: if None, all variables defined are returned,
             otherwise, it is a list contains that variables should be returned
    '''
    cur_vars = vars.copy()
    RE_AM_VAR_REF = re.compile(r'\$\((\w+?)\)')
    RE_AM_VAR = re.compile(r'^\s*(\w+)\s*=(.*)$')
    RE_AM_INCLUDE = re.compile(r'^\s*include\s+(\w+)')
    RE_AM_CONTINUING = re.compile(r'\\\s*$')
    RE_AM_IF = re.compile(r'^\s*if\s+(\w+)')
    RE_AM_ELSE = re.compile(r'^\s*else')
    RE_AM_ENDIF = re.compile(r'^\s*endif')
    def am_eval(cont):
        return RE_AM_VAR_REF.sub(lambda x: cur_vars.get(x.group(1), ''), cont)
    with open(path, 'r') as f:
        contents = f.readlines()
    #combine continuing lines
    i = 0
    ncont = []
    while i < len(contents):
        line = contents[i]
        if RE_AM_CONTINUING.search(line):
            line = RE_AM_CONTINUING.sub('', line)
            j = i + 1
            while j < len(contents) and RE_AM_CONTINUING.search(contents[j]):
                line += RE_AM_CONTINUING.sub('', contents[j])
                j += 1
            else:
                if j < len(contents):
                    line += contents[j]
            i = j
        else:
            i += 1
        ncont.append(line)

    #include, var define, var evaluation
    i = -1
    skip = False
    oldskip = []
    while i < len(ncont) - 1:
        i += 1
        line = ncont[i]
        mo = RE_AM_IF.search(line)
        if mo:
            oldskip.append(skip)
            skip = False if mo.group(1) in conds and conds[mo.group(1)] \
                         else True
            continue
        mo = RE_AM_ELSE.search(line)
        if mo:
            skip = not skip
            continue
        mo = RE_AM_ENDIF.search(line)
        if mo:
            skip = oldskip.pop()
            continue
        if not skip:
            mo = RE_AM_INCLUDE.search(line)
            if mo:
                cur_vars.update(read_vars_from_AM(am_eval(mo.group(1)), cur_vars, conds, None))
                continue
            mo = RE_AM_VAR.search(line)
            if mo:
                cur_vars[mo.group(1)] = am_eval(mo.group(2).strip())
                continue

    #filter:
    if filters != None:
        ret = {}
        for i in filters:
            ret[i] = cur_vars.get(i, '')
        return ret
    else:
        return cur_vars

def main(argv):
    srcroot = '..\\..'
    subdir = 'atk'
    gen_atk_filelist(srcroot, subdir, 'atk_list')
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
