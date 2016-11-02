<?xml version="1.0"?>

<xsl:transform version = "1.0" id="ingstr"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
>

<!-- It is one of the xslt transformation to generate ingestate structures.xml file from dbs.xml -->

    <xsl:output method="xml"/>
    <xsl:strip-space elements="*"/>

    <xsl:template match="/">
        <xsl:element name="structures">
            <xsl:text>&#010;</xsl:text>
            <xsl:apply-templates/>            
            <xsl:text>&#010;</xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>

    <xsl:template match="section">
        <xsl:text>  </xsl:text>
        <xsl:element name="terminalStructure">
            <xsl:attribute name="id">
                <xsl:value-of select="@id"/>
            </xsl:attribute>
            <xsl:attribute name="target">
                <xsl:text>data:</xsl:text>
                <xsl:value-of select="@id"/>
            </xsl:attribute>
            <!--
            <xsl:attribute name="targetLength">
                <xsl:value-of select="size"/>
            </xsl:attribute>
            -->
            <xsl:attribute name="targetLength">
                <xsl:value-of select="sum(field/@length)"/>
            </xsl:attribute>
            <xsl:text>&#010;</xsl:text>
            <xsl:apply-templates/>
            <xsl:text>  </xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>
    
    <xsl:template match="field">
        <xsl:text>    </xsl:text>
        <xsl:element name="field">
            <xsl:attribute name="id">
                <xsl:value-of select="@id"/>
            </xsl:attribute>
            <xsl:attribute name="offset">
                <xsl:value-of select="@offset"/>
            </xsl:attribute>
            <xsl:attribute name="length">
                <xsl:value-of select="@length"/>
            </xsl:attribute>
            <xsl:attribute name="format">
                <xsl:choose>
                    <xsl:when test="@type='ASCII'">
                        <xsl:text>R#00</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>L</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:attribute>
            <xsl:attribute name="type">
                <xsl:choose>
                    <xsl:when test="@type='ASCII'">
                        <xsl:text>string</xsl:text>
                    </xsl:when>
                    <xsl:when test="@type='binary'">
                        <xsl:text>bcd</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>integer</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:attribute>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>
    
    <xsl:template match="size"/>
</xsl:transform>
