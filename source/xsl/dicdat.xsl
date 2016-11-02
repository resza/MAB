<?xml version="1.0"?>

<xsl:transform version = "1.0" id="dicdat"
    extension-element-prefixes="uta"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
    xmlns:uta = "http://www.ingenico.com/xml/uta"
>

<!-- It is one of the xslt transformation to generate ingestate structures.xml file from dbs.xml -->

    <xsl:output method="xml"/>
    <xsl:strip-space elements="*"/>

    <xsl:key name="enumval" match="uta:enumKey" use="@uta:id" /> 

    <xsl:template match="/">
        <xsl:element name="structure">
            <xsl:text>&#010;</xsl:text>
            <xsl:text>  </xsl:text>
            <xsl:element name="dictionary">
                <xsl:text>&#010;</xsl:text>
                <xsl:apply-templates mode="dictionary"/>
                <xsl:text>  </xsl:text>
            </xsl:element>
            <xsl:text>&#010;</xsl:text>
            <xsl:text>&#010;</xsl:text>
            <xsl:text>  </xsl:text>
            <xsl:element name="data">
                <xsl:text>&#010;</xsl:text>
                <xsl:apply-templates mode="data"/>
                <xsl:text>  </xsl:text>
            </xsl:element>
            <xsl:text>&#010;</xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>

    <xsl:template match="uta:define"  mode="dictionary">
        <xsl:text>    </xsl:text>
        <xsl:element name="value">
            <xsl:attribute name="id">
                <xsl:value-of select="normalize-space(@uta:id)"/>
            </xsl:attribute>
            <xsl:attribute name="val">
                <xsl:value-of select="@uta:data"/>
            </xsl:attribute>
        </xsl:element>        
        <xsl:text>&#010;</xsl:text>
    </xsl:template>
    
    <xsl:template match="uta:enum"  mode="dictionary">
        <xsl:text>    </xsl:text>
        <xsl:element name="set">
            <xsl:attribute name="id">
                <xsl:value-of select="normalize-space(@uta:id)"/>
            </xsl:attribute>
            <xsl:text>&#010;</xsl:text>
            <xsl:text>      </xsl:text>
            <xsl:element name="value">
                <xsl:attribute name="id">
                    <xsl:value-of select="normalize-space(@uta:id)"/>
                    <xsl:text>Beg</xsl:text>
                </xsl:attribute>
                <xsl:attribute name="val">
                    <xsl:text>0</xsl:text>
                </xsl:attribute>
            </xsl:element>
            <xsl:text>&#010;</xsl:text>
            
            <xsl:apply-templates mode="dictionary"/>
            
            <xsl:text>      </xsl:text>
            <xsl:element name="value">
                <xsl:attribute name="id">
                    <xsl:value-of select="normalize-space(@uta:id)"/>
                    <xsl:text>End</xsl:text>
                </xsl:attribute>
                <xsl:attribute name="val">
                    <xsl:value-of select="count(uta:enumKey)+1"/>
                </xsl:attribute>
            </xsl:element>
            <xsl:text>&#010;</xsl:text>
            <xsl:text>    </xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>
        
    <xsl:template match="uta:enumKey" mode="dictionary">
        <xsl:text>      </xsl:text>
        <xsl:element name="value">
            <xsl:attribute name="id">
                <xsl:value-of select="normalize-space(@uta:id)"/>
            </xsl:attribute>
            <xsl:attribute name="val">
                <xsl:value-of select="@uta:data"/>
            </xsl:attribute>
        </xsl:element>        
        <xsl:text>&#010;</xsl:text>
        <xsl:apply-templates/>
    </xsl:template>
        
    <xsl:template match="uta:keySpace" mode="dictionary"/>

    <xsl:template match="uta:keySpace" mode="data">
        <xsl:apply-templates mode="data"/>
    </xsl:template>

    <xsl:template match="uta:dfsFile" mode="data">
        <xsl:text>    </xsl:text>
        <xsl:element name="section">
            <xsl:attribute name="id">
                <xsl:value-of select="normalize-space(@uta:id)"/>
            </xsl:attribute>
            <xsl:text>&#010;</xsl:text>
            <xsl:apply-templates mode="data"/>
            <xsl:text>    </xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>

    <xsl:template match="uta:record" mode="data">
        <xsl:text>      </xsl:text>
        <xsl:element name="record">
            <xsl:attribute name="id">
                <xsl:value-of select="normalize-space(@uta:id)"/>
            </xsl:attribute>
            <xsl:attribute name="dim">
                <xsl:choose>
                    <xsl:when test="@uta:multi!=''">
                        <xsl:value-of select="count(uta:data/uta:instance)"/>
                    </xsl:when>
                    <xsl:when test="@uta:dim!=''">
                        <xsl:value-of select="@uta:dim"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>1</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:attribute>
            <xsl:text>&#010;</xsl:text>
            <xsl:apply-templates mode="data"/>
            <xsl:text>      </xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>

    <xsl:template match="uta:field" mode="data">
        <xsl:text>        </xsl:text>
        <xsl:element name="field">
            <xsl:attribute name="id">
                <xsl:value-of select="normalize-space(@uta:id)"/>
            </xsl:attribute>
            <xsl:attribute name="length">
                <xsl:choose>
                    <xsl:when test="@uta:type='char'">
                        <xsl:text>1</xsl:text>
                    </xsl:when>
                    <xsl:when test="@uta:type='byte'">
                        <xsl:text>1</xsl:text>
                    </xsl:when>
                    <xsl:when test="@uta:type='word'">
                        <xsl:text>2</xsl:text>
                    </xsl:when>
                    <xsl:when test="@uta:type='card'">
                        <xsl:text>4</xsl:text>
                    </xsl:when>
                    <xsl:when test="@uta:type='binary'">
                        <xsl:text>1+</xsl:text>
                        <xsl:value-of select="@uta:length" /> 
                    </xsl:when>
                    <xsl:when test="@uta:type='ASCII'">
                        <xsl:value-of select="@uta:length" /> 
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>?unknown?</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:attribute>
            <xsl:attribute name="type">
                <xsl:value-of select="@uta:type"/>
            </xsl:attribute>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>
    
    <xsl:template match="uta:table" mode="data">
        <xsl:text>      </xsl:text>
        <xsl:element name="table">
            <xsl:attribute name="id">
                <xsl:value-of select="@uta:id"/>
            </xsl:attribute>
            <xsl:attribute name="dim">
                <xsl:value-of select="count(uta:row)"/>
            </xsl:attribute>
            <xsl:text>&#010;</xsl:text>
            <xsl:apply-templates mode="data"/>
            <xsl:text>      </xsl:text>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>
    
    <xsl:template match="uta:row" mode="data">
        <xsl:text>        </xsl:text>
        <xsl:element name="field">
            <xsl:attribute name="id">
                <xsl:value-of select="@uta:id"/>
            </xsl:attribute>
            <xsl:attribute name="length">
                <xsl:choose>
                    <xsl:when test="@uta:type='char'">
                        <xsl:text>1</xsl:text>
                    </xsl:when>
                    <xsl:when test="../@uta:type='byte'">
                        <xsl:text>1</xsl:text>
                    </xsl:when>
                    <xsl:when test="../@uta:type='word'">
                        <xsl:text>2</xsl:text>
                    </xsl:when>
                    <xsl:when test="../@uta:type='card'">
                        <xsl:text>4</xsl:text>
                    </xsl:when>
                    <xsl:when test="../@uta:type='binary'">
                        <xsl:text>1+</xsl:text>
                        <xsl:value-of select="../@uta:length" /> 
                    </xsl:when>
                    <xsl:when test="../@uta:type='ASCII'">
                        <xsl:value-of select="../@uta:length" /> 
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>?unknown?</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:attribute>
            <xsl:attribute name="type">
                <xsl:value-of select="../@uta:type"/>
            </xsl:attribute>
        </xsl:element>
        <xsl:text>&#010;</xsl:text>
    </xsl:template>

    <xsl:template match="uta:volatile" mode="dictionary"/>
    <xsl:template match="uta:derivated" mode="dictionary"/>
    <xsl:template match="uta:rptLayout" mode="dictionary"/>
    <xsl:template match="uta:volatile" mode="data"/>
    <xsl:template match="uta:derivated" mode="data"/>
    <xsl:template match="uta:rptLayout" mode="data"/>
    
</xsl:transform>
