<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"     xmlns:dyn="http://exslt.org/dynamic"   xmlns:math="http://exslt.org/math" extension-element-prefixes="dyn math" version="1.0">

<xsl:output method="text" indent="no"/>
  <xsl:template match="/">
    <xsl:apply-templates select="/root/file"/>
  </xsl:template>


  <xsl:template match="variant">
    <xsl:if test="$variant = @type">
      <xsl:apply-templates select="child::node()"/>
    </xsl:if>  
  </xsl:template>

  <xsl:template match="repl">
    <xsl:apply-templates select="/root/repl-text">
      <xsl:with-param name="pos" select="count(preceding-sibling::*[name()='repl'])"/>
    </xsl:apply-templates>
  </xsl:template>
  <xsl:template match="range">
    <xsl:param name="pos"/>
    <xsl:if test="@min &lt;= $pos and @max &gt;= $pos">
      <xsl:apply-templates select="child::node()">
        <xsl:with-param name="pos" select="$pos"/>
      </xsl:apply-templates>
    </xsl:if>
  </xsl:template>
  <xsl:template match="eval">
    <xsl:param name="pos"/>
    <xsl:value-of select="dyn:evaluate(.)"/>
  </xsl:template>
</xsl:stylesheet>
