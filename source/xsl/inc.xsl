<?xml version="1.0"?>

<xsl:stylesheet version = "1.0" id="inc"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
>

<xsl:include href="coding.xsl"/>
<xsl:strip-space elements="*"/>

<xsl:variable name="az">abcdefghijklmnopqrstuvwxyz</xsl:variable>
<xsl:variable name="AZ">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>

<!-- Repeat the string 'str' 'cnt' times -->
<xsl:template name="repeat-string">
  <xsl:param name="str"/><!-- The string to repeat -->
  <xsl:param name="cnt"/><!-- The number of times to repeat the string -->
  <xsl:param name="pfx"/><!-- The prefix to add to the string -->
  <xsl:choose>
    <xsl:when test="$cnt = 0">
      <xsl:value-of select="$pfx"/>
    </xsl:when>
    <xsl:when test="$cnt mod 2 = 1">
    <xsl:call-template name="repeat-string">
	  <xsl:with-param name="str" select="concat($str,$str)"/>
	  <xsl:with-param name="cnt" select="($cnt - 1) div 2"/>
	  <xsl:with-param name="pfx" select="concat($pfx,$str)"/>
	</xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
	<xsl:call-template name="repeat-string">
	  <xsl:with-param name="str" select="concat($str,$str)"/>
	  <xsl:with-param name="cnt" select="$cnt div 2"/>
	  <xsl:with-param name="pfx" select="$pfx"/>
	</xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>