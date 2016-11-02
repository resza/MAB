<?xml version="1.0"?>

<xsl:transform version = "1.0" id="dfth"
    extension-element-prefixes="uta"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
    xmlns:uta = "http://www.ingenico.com/xml/uta"
>

<xsl:include href="inc.xsl"/>

<xsl:template match = "/">
    <xsl:text>#ifndef __DFT_H&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>//this file is included in dft.c only&#010;</xsl:text>
    <xsl:text>//it is not visible for other source files&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates/>

    <xsl:text>#define __DFT_H&#010;</xsl:text>
    <xsl:text>#endif&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk"/>
<xsl:template match="uta:enum"/>
<xsl:template match="uta:keySpace"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:dfsFile"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:volatile"/>
<xsl:template match="uta:derivated"/>
<xsl:template match="uta:rptLayout"/>

<xsl:template match="uta:table">
    <xsl:choose>
      <xsl:when test="@uta:dim!=''"/>
      <xsl:otherwise>
        <xsl:text>static char *dft</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>[DIM(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg,</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>End)]={</xsl:text>
        <xsl:text>&#010;</xsl:text>    
        <xsl:apply-templates/>
        <xsl:text>};&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>    
      </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="uta:row">
    <xsl:text>    </xsl:text>
    <xsl:value-of select="@uta:data"/>
    <xsl:text>, //</xsl:text>
    <xsl:choose>
      <xsl:when test="@uta:id!=''"><xsl:value-of select="@uta:id"/></xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="../@uta:id"/>
        <xsl:value-of select="position()-1"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#010;</xsl:text>    
</xsl:template>

<xsl:template match="uta:record">
    <xsl:choose>
        <xsl:when test="@uta:dim!='' or @uta:multi!=''">
            <xsl:if test="count(uta:data)!=0">
                <xsl:text>typedef struct s</xsl:text>
                <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>Record{&#010;</xsl:text>
                <xsl:apply-templates mode="multidcl"/>
                <xsl:text>}t</xsl:text>
                <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>Record;&#010;&#010;</xsl:text>
                <xsl:apply-templates mode="multi"/>
            </xsl:if>
        </xsl:when>
        <xsl:otherwise>
            <xsl:apply-templates mode="single"/>
            <xsl:text>&#010;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="uta:field" mode="single">
    <xsl:if test="@uta:data!=''">
        <xsl:text>#define dft</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="@uta:data"/>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:field" mode="multidcl">
    <xsl:text>    </xsl:text>
    <xsl:choose>
        <xsl:when test="@uta:type='ASCII'">
            <xsl:text>char *_</xsl:text>
            <xsl:value-of select="@uta:id"/>
        </xsl:when>
        <xsl:when test="@uta:type='binary'">
            <xsl:text>byte *_</xsl:text>
            <xsl:value-of select="@uta:id"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="@uta:type"/>
            <xsl:text> _</xsl:text>
            <xsl:value-of select="@uta:id"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="@uta:repeat!=''">
        <xsl:text>[</xsl:text>
        <xsl:value-of select="@uta:repeat"/>
        <xsl:text>]</xsl:text>
    </xsl:if>
    <xsl:text>;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:data" mode="multi">
    <xsl:text>static t</xsl:text>
    <xsl:value-of select="translate(substring(parent::node()/@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(parent::node()/@uta:id,2)"/>
    <xsl:text>Record dft</xsl:text>
    <xsl:value-of select="translate(substring(parent::node()/@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(parent::node()/@uta:id,2)"/>
    <xsl:text>[</xsl:text>
    <xsl:if test="parent::node()/@uta:dim!=''">
<!--
        <xsl:value-of select="parent::node()/@uta:dim"/>
-->
    </xsl:if>
    <xsl:if test="parent::node()/@uta:multi!=''">
        <xsl:text>DIM(</xsl:text>
        <xsl:value-of select="parent::node()/@uta:multi"/>
        <xsl:text>Beg,</xsl:text>
        <xsl:value-of select="parent::node()/@uta:multi"/>
        <xsl:text>End)</xsl:text>
    </xsl:if>
    <xsl:text>]={</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:apply-templates mode="multi"/>
    <xsl:text>};&#010;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:instance" mode="multi">
    <xsl:text>    {</xsl:text>
    <xsl:apply-templates mode="multi"/>
    <xsl:text>},</xsl:text>
    <xsl:text> //</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match = "uta:span" mode="multi">
    <xsl:text>{</xsl:text>
    <xsl:apply-templates mode="multi"/>
    <xsl:text>},</xsl:text>
</xsl:template>

<xsl:template match = "uta:col" mode="multi">
    <xsl:value-of select="@uta:data"/>    
    <xsl:if test="not(position()=last())">
        <xsl:text>,</xsl:text>
    </xsl:if>
</xsl:template>

</xsl:transform>
