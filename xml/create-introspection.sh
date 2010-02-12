#!/bin/sh

xsltproc --xinclude remove-annotations.xsl Accessibility.xml -o Processed.xml
python versioned-introspection.py
