<?xml version="1.0"?>

<xsl:transform version = "1.0" id="scnh"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

  <xsl:include href="inc.xsl"/>

  <xsl:template match="/">
    <xsl:text>#ifndef __PIC_H&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  
    <xsl:text>enum ePic{&#010;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>    picEnd&#010;</xsl:text>
    <xsl:text>};&#010;</xsl:text>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>const unsigned char *picBmpPtr(unsigned short pic);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>#define __PIC_H&#010;</xsl:text>
    <xsl:text>#endif&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  </xsl:template>

  <xsl:template match="uta:dbk"/>
  <xsl:template match="uta:keySpace"/>
  <xsl:template match="uta:enum"/>
  <xsl:template match="uta:rptLayout"/>
  <xsl:template match="uta:scenes"><xsl:apply-templates/></xsl:template>

  <xsl:template match="uta:shape">
    <xsl:if test="@uta:type='shpTile'">
      <xsl:if test="@uta:pic!=''">
          <xsl:text>    pic</xsl:text>
          <xsl:value-of select="translate(substring(@uta:pic,1,1),$az,$AZ)"/>
          <xsl:value-of select="substring(@uta:pic,2,string-length(@uta:pic)-5)"/>
          <xsl:text>,&#010;</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:template>

</xsl:transform>
