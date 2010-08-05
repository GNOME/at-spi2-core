#!/bin/sh

xsltproc --xinclude remove-annotations.xsl Accessibility.xml >Processed.xml
python versioned-introspection.py
