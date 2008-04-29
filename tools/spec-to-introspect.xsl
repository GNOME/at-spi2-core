<!-- Remove Telepathy extensions from a D-Bus interface, trying to leave only
DTD-compliant introspection XML. The output still needs to be subjected to the
following sed filter:
sed -e 's@xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec.extensions-v0"@@g'

The master copy of this stylesheet is in the Telepathy spec repository -
please make any changes there.

Copyright (C) 2006, 2007 Collabora Limited

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:tp="http://telepathy.freedesktop.org/wiki/DbusSpec#extensions-v0"
  exclude-result-prefixes="tp">

  <xsl:template match="*">
    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:if test="not(starts-with(name(), 'tp:'))">
          <xsl:copy/>
        </xsl:if>
      </xsl:for-each>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="tp:*"/>
  <xsl:template match="text()"/>

  <xsl:output method="xml" indent="yes" encoding="UTF-8"
    omit-xml-declaration="no"
    doctype-system="http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd"
    doctype-public="-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" />

</xsl:stylesheet>

<!-- vim:set sw=2 sts=2 et: -->
