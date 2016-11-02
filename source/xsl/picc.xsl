<?xml version="1.0"?>

<xsl:transform version = "1.0" id="scnh"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

  <xsl:include href="inc.xsl"/>

  <xsl:template match="/">
    <xsl:text>#include "str.h"&#010;</xsl:text>
    <xsl:apply-templates mode="include"/>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>const unsigned char *picBmpPtr(unsigned short pic){&#010;</xsl:text>
    <xsl:text>    const char *ptr;&#010;</xsl:text>
    <xsl:text>    ptr= 0;&#010;</xsl:text>
    <xsl:text>    switch(pic){&#010;</xsl:text>
    <xsl:apply-templates mode="switch"/>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return ptr;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  </xsl:template>

  <xsl:template match="uta:dbk" mode="include"/>
  <xsl:template match="uta:keySpace" mode="include"/>
  <xsl:template match="uta:enum" mode="include"/>
  <xsl:template match="uta:rptLayout" mode="include"/>
  <xsl:template match="uta:scenes" mode="include"><xsl:apply-templates mode="include"/></xsl:template>

  <xsl:template match="uta:shape" mode="include">
    <xsl:if test="@uta:type='shpTile'">
      <xsl:if test="@uta:pic!=''">
          <xsl:text>#include "</xsl:text>
          <xsl:value-of select="substring(@uta:pic,1,string-length(@uta:pic)-4)"/>
          <xsl:text>.c"&#010;</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:template>

  <xsl:template match="uta:dbk" mode="switch"/>
  <xsl:template match="uta:keySpace" mode="switch"/>
  <xsl:template match="uta:enum" mode="switch"/>
  <xsl:template match="uta:rptLayout" mode="switch"/>
  <xsl:template match="uta:scenes" mode="switch"><xsl:apply-templates mode="switch"/></xsl:template>

  <xsl:template match="uta:shape" mode="switch">
    <xsl:if test="@uta:type='shpTile'">
      <xsl:if test="@uta:pic!=''">
          <xsl:text>        case pic</xsl:text>
          <xsl:value-of select="translate(substring(@uta:pic,1,1),$az,$AZ)"/>
          <xsl:value-of select="substring(@uta:pic,2,string-length(@uta:pic)-5)"/>
          <xsl:text>: ptr= BMP_</xsl:text>
          <xsl:value-of select="substring(@uta:pic,1,string-length(@uta:pic)-4)"/>
          <xsl:text>; break;&#010;</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:template>

</xsl:transform>
