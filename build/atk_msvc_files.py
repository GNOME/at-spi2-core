#! /usr/bin/python

# Expand The Visual Studio Files from their templates

import os
import optparse
import sys

from msvcfiles import parent_dir
from msvcfiles import check_output_type
from msvcfiles import generate_src_list
from msvcfiles import gen_vs9_project
from msvcfiles import gen_vs10_project
from msvcfiles import generate_nmake_makefiles
from msvcfiles import gen_vs_inst_list

def main(argv):
    parser = optparse.OptionParser()
    parser.add_option('-t', '--type', dest='output_type', metavar='string', action='store', help='Visual Studio output build file type to generate ("nmake-exe","vs9","vs10")')
    opt, args = parser.parse_args(argv)

    srcroot = parent_dir(__file__)
    output_type = check_output_type (opt.output_type)
    if (output_type == -1):
        sys.exit()

    elif (output_type == 3):
        # Generate the executable list from tests/
        test_filters_progs = ['noinst_PROGRAMS']
        test_filters_conds = {}
        test_src_dir = os.path.join(srcroot, 'tests')
        test_progs = generate_src_list (srcroot, test_src_dir, test_filters_progs, test_filters_conds, False, None)
        generate_nmake_makefiles(srcroot, test_src_dir, "test", "testatk_vc.mak", test_progs)

    elif (output_type == 1 or output_type == 2):
        # Generate the ATK MSVC 2008 or 2010 project files
        atk_filters_src = ['libatk_1_0_la_SOURCES']
        atk_filters_conds = {}
        atk_src_dir = os.path.join(srcroot, 'atk')
        atk_src_files = generate_src_list (srcroot, atk_src_dir, atk_filters_src, atk_filters_conds, True, None)
        if (output_type == 1):
            gen_vs9_project ('atk', srcroot, 'atk', atk_src_files)
        else:
            gen_vs10_project ('atk', srcroot, 'atk', atk_src_files)


        # Generate the ATK headers list to "install" for MSVC 2008/2010
        atk_filters_h_conds = {}
        atk_filters_h = ['libatkinclude_HEADERS']
        atk_h_files_raw = generate_src_list (srcroot, atk_src_dir, atk_filters_h, atk_filters_h_conds, False, None)
        atk_h_files =  [files.replace('/atk/', '') for files in atk_h_files_raw]

        srcdirs = ['atk']

        inst_h_lists = [atk_h_files]

        inst_h_dirs = ['include\\atk-$(ApiVersion)\\atk']

        if (output_type == 1):
            gen_vs_inst_list ('atk', srcroot, srcdirs, inst_h_lists, inst_h_dirs, True)
        else:
            gen_vs_inst_list ('atk', srcroot, srcdirs, inst_h_lists, inst_h_dirs, False)

    else:
         raise Exception ("Somehow your output_type is wrong.\nShould not have seen this message!")

if __name__ == '__main__':
    sys.exit(main(sys.argv))
