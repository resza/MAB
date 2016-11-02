<?xml version="1.0"?>

<xsl:transform version = "1.0" id="scnh"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

  <xsl:include href="inc.xsl"/>
  <xsl:key name="drawing" match="uta:drawing" use="@uta:id" /> 

  <xsl:template match="/">
    <xsl:text>#ifndef __SCN_H&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  
    <xsl:text>//this file is included in scn.c only&#010;</xsl:text>
    <xsl:text>//it is not visible for other source files&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  
    <xsl:text>#include "str.h"&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  
    <xsl:apply-templates/>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>//array containing all the shapes&#010;</xsl:text>
    <xsl:text>static tShape _shapes[DIM(scnBeg,scnEnd)];&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>#define __SCN_H&#010;</xsl:text>
    <xsl:text>#endif&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  </xsl:template>

  <xsl:template match="uta:dbk"/>
  <xsl:template match="uta:keySpace"/>
  <xsl:template match="uta:enum"/>
  <xsl:template match="uta:scenes"><xsl:apply-templates/></xsl:template>

  <xsl:template match="uta:scene">
    <xsl:text>//Scene </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text> declarations: </xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#010;</xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:shape"><xsl:apply-templates/></xsl:template>
  <xsl:template match="uta:drawing"><xsl:apply-templates/></xsl:template>

  <xsl:template match="uta:shape">
    <xsl:choose>
      <xsl:when test="@uta:type='shpPixel'"><xsl:text>static tPixel _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpLine'"><xsl:text>static tLine _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpBar'"><xsl:text>static tBar _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpText'"><xsl:text>static tText _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpTile'"><xsl:text>static tTile _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpBox'"><xsl:text>static tBox _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpCircle'"><xsl:text>static tCircle _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpDisc'"><xsl:text>static tDisc _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpBezier'"><xsl:text>static tBezier _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpButton'"><xsl:text>static tButton _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpSlider'"><xsl:text>static tSlider _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpSelector'"><xsl:text>static tSelector _</xsl:text></xsl:when>
      <xsl:when test="@uta:type='shpVideo'"><xsl:text>static tVideo _</xsl:text></xsl:when>
      <xsl:otherwise><xsl:text>UNDER CONSTRUCTION</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>; //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
  </xsl:template>

  <xsl:template match="uta:drawing">
    <xsl:text>static tDrawing _</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>; //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    
  </xsl:template>

  <xsl:template match="uta:gui">
    <xsl:text>//GUI element </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text> declarations: </xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>static tInput _gui[DIM(guiBeg,guiEnd)];&#010;</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template match="uta:input">
    <xsl:if test="count(uta:virtual)!='0'">
      <xsl:text>static tVmtGui _vmt</xsl:text>
      <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
      <xsl:value-of select="substring(@uta:id,2)"/>
      <xsl:text>;&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="uta:rptLayout"/>

</xsl:transform>
