#!/usr/bin/env python3
#
# Takes a DBus XML file and writes out introspection.[ch] files for inclusion
# in C code.  Both files are written to the OUTPUT_DIRECTORY as per below.
#
# Usage: python3 versioned-introspection.py INPUT_FILE.XML OUTPUT_DIRECTORY

import os
import sys
from xml.etree import ElementTree

CTEMPLATE = """
/*
 * This file has been auto-generated from the introspection data available
 * in the at-spi2-core repository. The D-Bus procol is defined in this
 * repository, which can be found at:
 *
 * http://download.gnome.org/sources/at-spi2-core/0.1/
 *
 * DO NOT EDIT.
 */

%s
"""

HTEMPLATE = """
/*
 * This file has been auto-generated from the introspection data available
 * in the at-spi2-core repository. The D-Bus procol is defined in this
 * repository, which can be found at:
 *
 * http://download.gnome.org/sources/at-spi2-core/0.1/
 *
 * DO NOT EDIT.
 */

#ifndef SPI_INTROSPECTION_DATA_H_
#define SPI_INTROSPECTION_DATA_H_

%s

#endif /* SPI_INTROSPECTION_DATA_H_ */
"""

DECTEMPLATE = """
extern const char *%s;
"""

DEFTEMPLATE = """
const char *%s = 
%s;
"""

VERSION = "0.1.7"

def convert_name (name):
    return "spi_" + name.replace (".", "_")

def convert_contents (contents):
    contents = contents.replace ("\"", "\\\"")
    literals = ["\"%s\"" % (line) for line in contents.split ("\n")]
    return "\n".join (literals)

def generate_introspection (input_filename, output_directory):
    #Open the XML file and process includes.
    tree = ElementTree.parse (input_filename)
    root = tree.getroot ()

    #Open the output files.
    c_filename = os.path.join(output_directory, "introspection.c")
    h_filename = os.path.join(output_directory, "introspection.h")
    cfile = open (c_filename, "w")
    hfile = open (h_filename, "w")

    ccontents = ""
    hcontents = ""

    for itf in root.findall ("node/interface"):
        #Get and convert the name of the interface.
        name = convert_name (itf.attrib["name"])

        #Create the introspection string with version information.
        itf.attrib["version"] = VERSION
        contents = convert_contents (ElementTree.tostring (itf, encoding="unicode"))

        hcontents += DECTEMPLATE % (name)
        ccontents += DEFTEMPLATE % (name, contents)

    cfile.write (CTEMPLATE % (ccontents))
    hfile.write (HTEMPLATE % (hcontents))

    cfile.close ()
    hfile.close ()  
        
if __name__ == "__main__":
    argv = sys.argv

    if len (argv) != 3:
        print("usage: versioned-introspection.py INPUT.XML OUTPUT_DIR", file=sys.stderr)
        sys.exit(1)

    input_filename = sys.argv[1]
    output_directory = sys.argv[2]

    generate_introspection (input_filename, output_directory)
