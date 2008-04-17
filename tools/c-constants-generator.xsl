<!-- Stylesheet to extract C enumerations from the AT-SPI spec.

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:tp="http://telepathy.freedesktop.org/wiki/DbusSpec#extensions-v0"
  xmlns:str="http://exslt.org/strings"
  exclude-result-prefixes="str tp">

  <xsl:output method="text" indent="no" encoding="ascii"/>

  <xsl:param name="mixed-case-prefix" select="''"/>

  <xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>
  <xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>

  <xsl:variable name="lower-case-prefix" select="translate($mixed-case-prefix, $upper, $lower)"/>


  <xsl:template match="tp:flags">
    <xsl:variable name="name">
      <xsl:choose>
        <xsl:when test="@plural">
          <xsl:value-of select="@plural"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="value-prefix">
      <xsl:choose>
        <xsl:when test="@singular">
          <xsl:value-of select="@singular"/>
        </xsl:when>
        <xsl:when test="@value-prefix">
          <xsl:value-of select="@value-prefix"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:text>/**&#10;</xsl:text>
    <xsl:text> *&#10;</xsl:text>
    <xsl:value-of select="concat($mixed-case-prefix, $name)"/>
    <xsl:text>:&#10;</xsl:text>
    <xsl:apply-templates mode="flag-or-enumvalue-gtkdoc">
      <xsl:with-param name="value-prefix" select="$value-prefix"/>
    </xsl:apply-templates>
    <xsl:text> *&#10;</xsl:text>
    <xsl:if test="tp:docstring">
      <xsl:text> * &lt;![CDATA[</xsl:text>
      <xsl:value-of select="translate(string (tp:docstring), '&#13;&#10;', '  ')"/>
      <xsl:text>]]&gt;&#10;</xsl:text>
      <xsl:text> *&#10;</xsl:text>
    </xsl:if>
    <xsl:text> * Bitfield/set of flags generated from the AT-SPI specification.&#10;</xsl:text>
    <xsl:text> */&#10;</xsl:text>
    <xsl:text>typedef enum {&#10;</xsl:text>
    <xsl:apply-templates>
      <xsl:with-param name="value-prefix" select="$value-prefix"/>
    </xsl:apply-templates>
    <xsl:text>} </xsl:text>
    <xsl:value-of select="concat($mixed-case-prefix, $name)"/>
    <xsl:text>;&#10;</xsl:text>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="text()" mode="flag-or-enumvalue-gtkdoc"/>

  <xsl:template match="tp:enumvalue" mode="flag-or-enumvalue-gtkdoc">
    <xsl:param name="value-prefix"/>
    <xsl:variable name="suffix">
      <xsl:choose>
        <xsl:when test="ancestor::interface">
          <xsl:value-of select="concat(str:replace(ancestor::interface/@name, 'org.freedesktop.accessibility.', ''), '_', @suffix)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@suffix"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:text> * @</xsl:text>
    <xsl:value-of select="concat($mixed-case-prefix, $suffix)"/>
    <xsl:text>: &lt;![CDATA[</xsl:text>
    <xsl:value-of select="translate(string(tp:docstring), '&#13;&#10;', '  ')"/>
    <xsl:text>]]&gt;&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="tp:flag" mode="flag-or-enumvalue-gtkdoc">
    <xsl:param name="value-prefix"/>
    <xsl:text> * @</xsl:text>
    <xsl:value-of select="translate(concat($mixed-case-prefix, $value-prefix, '_', @suffix), $lower, $upper)"/>
    <xsl:text>: &lt;![CDATA[</xsl:text>
    <xsl:value-of select="translate(string(tp:docstring), '&#13;&#10;', '  ')"/>
    <xsl:text>]]&gt;&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="tp:enum">
    <xsl:variable name="name">
      <xsl:choose>
        <xsl:when test="ancestor::interface">
          <xsl:value-of select="concat(str:replace(ancestor::interface/@name, 'org.freedesktop.accessibility.', ''), '_', @name)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="value-prefix">
      <xsl:choose>
        <xsl:when test="@singular">
          <xsl:value-of select="@singular"/>
        </xsl:when>
        <xsl:when test="@value-prefix">
          <xsl:value-of select="@value-prefix"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="name-plural">
      <xsl:choose>
        <xsl:when test="@plural">
          <xsl:value-of select="@plural"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@name"/><xsl:text>s</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:text>/**&#10;</xsl:text>
    <xsl:text> *&#10;</xsl:text>
    <xsl:value-of select="concat($mixed-case-prefix, $name)"/>
    <xsl:text>:&#10;</xsl:text>
    <xsl:apply-templates mode="flag-or-enumvalue-gtkdoc">
      <xsl:with-param name="value-prefix" select="$value-prefix"/>
    </xsl:apply-templates>
    <xsl:text> *&#10;</xsl:text>
    <xsl:if test="tp:docstring">
      <xsl:text> * &lt;![CDATA[</xsl:text>
      <xsl:value-of select="translate(string (tp:docstring), '&#13;&#10;', '  ')"/>
      <xsl:text>]]&gt;&#10;</xsl:text>
      <xsl:text> *&#10;</xsl:text>
    </xsl:if>
    <xsl:text> * Bitfield/set of flags generated from the AT-SPI specification.&#10;</xsl:text>
    <xsl:text> */&#10;</xsl:text>
    <xsl:text>typedef enum {&#10;</xsl:text>
    <xsl:apply-templates>
      <xsl:with-param name="value-prefix" select="$value-prefix"/>
    </xsl:apply-templates>
    <xsl:text>} </xsl:text>
    <xsl:value-of select="concat($mixed-case-prefix, $name)"/>
    <xsl:text>;&#10;</xsl:text>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>/**&#10;</xsl:text>
    <xsl:text> * NUM_</xsl:text>
    <xsl:value-of select="translate(concat($mixed-case-prefix, $name-plural), $lower, $upper)"/>
    <xsl:text>:&#10;</xsl:text>
    <xsl:text> *&#10;</xsl:text>
    <xsl:text> * 1 higher than the highest valid value of #</xsl:text>
    <xsl:value-of select="concat($mixed-case-prefix, $name)"/>
    <xsl:text>.&#10;</xsl:text>
    <xsl:text> */&#10;</xsl:text>
    <xsl:text>#define NUM_</xsl:text>
    <xsl:value-of select="translate(concat($mixed-case-prefix, $name-plural), $lower, $upper)"/>
    <xsl:text> (</xsl:text>
    <xsl:value-of select="tp:enumvalue[position() = last()]/@value"/>
    <xsl:text>+1)&#10;</xsl:text>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="tp:flags/tp:flag">
    <xsl:param name="value-prefix"/>
    <xsl:variable name="suffix">
      <xsl:choose>
        <xsl:when test="@suffix">
          <xsl:value-of select="@suffix"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="name" select="translate(concat($mixed-case-prefix, $value-prefix, '_', $suffix), $lower, $upper)"/>

    <xsl:if test="@name and @suffix and @name != @suffix">
      <xsl:message terminate="yes">
        <xsl:text>Flag name </xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text> != suffix </xsl:text>
        <xsl:value-of select="@suffix"/>
        <xsl:text>&#10;</xsl:text>
      </xsl:message>
    </xsl:if>
    <xsl:text>    </xsl:text>
    <xsl:value-of select="$name"/>
    <xsl:text> = </xsl:text>
    <xsl:value-of select="@value"/>
    <xsl:text>,&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="tp:enum/tp:enumvalue">
    <xsl:param name="value-prefix"/>
    <xsl:variable name="suffix">
      <xsl:choose>
        <xsl:when test="ancestor::interface">
          <xsl:value-of select="concat(str:replace(ancestor::interface/@name, 'org.freedesktop.accessibility.', ''), '_', @suffix)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@suffix"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="name" select="concat($mixed-case-prefix, $suffix)"/>

    <xsl:if test="@name and @suffix and @name != @suffix">
      <xsl:message terminate="yes">
        <xsl:text>Enumvalue name </xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text> != suffix </xsl:text>
        <xsl:value-of select="@suffix"/>
        <xsl:text>&#10;</xsl:text>
      </xsl:message>
    </xsl:if>

    <xsl:if test="preceding-sibling::tp:enumvalue and number(preceding-sibling::tp:enumvalue[1]/@value) > number(@value)">
      <xsl:message terminate="yes">
        <xsl:text>Enum values must be in ascending numeric order, but </xsl:text>
        <xsl:value-of select="$name"/>
        <xsl:text> is less than the previous value</xsl:text>
      </xsl:message>
    </xsl:if>

    <xsl:text>    </xsl:text>
    <xsl:value-of select="$name"/>
    <xsl:text>,&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="tp:flag">
    <xsl:message terminate="yes">tp:flag found outside tp:flags&#10;</xsl:message>
  </xsl:template>

  <xsl:template match="tp:enumvalue">
    <xsl:message terminate="yes">tp:enumvalue found outside tp:enum&#10;</xsl:message>
  </xsl:template>

  <xsl:template match="text()"/>

  <xsl:template match="/tp:spec">
    <xsl:if test="$mixed-case-prefix = ''">
      <xsl:message terminate="yes">
        <xsl:text>mixed-case-prefix param must be set&#10;</xsl:text>
      </xsl:message>
    </xsl:if>

    <xsl:text>/* Generated from </xsl:text>
    <xsl:value-of select="tp:title"/>
    <xsl:if test="tp:version">
      <xsl:text>, version </xsl:text>
      <xsl:value-of select="tp:version"/>
    </xsl:if>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>&#10;</xsl:text>
    <xsl:for-each select="tp:copyright">
      <xsl:value-of select="."/>
      <xsl:text>&#10;</xsl:text>
    </xsl:for-each>
    <xsl:value-of select="tp:license"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:value-of select="tp:docstring"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:text> */&#10;</xsl:text>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>#ifdef __cplusplus&#10;</xsl:text>
    <xsl:text>extern "C" {&#10;</xsl:text>
    <xsl:text>#endif&#10;</xsl:text>
    <xsl:text>&#10;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>#ifdef __cplusplus&#10;</xsl:text>
    <xsl:text>}&#10;</xsl:text>
    <xsl:text>#endif&#10;</xsl:text>
  </xsl:template>

</xsl:stylesheet>

<!-- vim:set sw=2 sts=2 et noai noci: -->
