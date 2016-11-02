<?xml version="1.0"?>

<xsl:transform version = "1.0" id="scnh"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

  <xsl:include href="inc.xsl"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="uta:dbk"/>
  <xsl:template match="uta:keySpace"/>
  <xsl:template match="uta:enum"/>
  <xsl:template match="uta:rptLayout"/>
  <xsl:template match="uta:scenes"><xsl:apply-templates/></xsl:template>

  <xsl:template match="uta:shape">
    <xsl:if test="@uta:type='shpTile'">
      <xsl:if test="@uta:pic!=''">
          <xsl:text>icnvBmp.exe /r /f</xsl:text>
          <xsl:value-of select="@uta:pic"/>
          <xsl:text>&#010;</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:template>

</xsl:transform>
