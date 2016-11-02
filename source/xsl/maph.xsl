<?xml version="1.0"?>

<xsl:transform version = "1.0" id="maph"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

<xsl:include href="inc.xsl"/>

<xsl:template match="/">
  <xsl:text>#ifndef __MAP_H&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

  <xsl:text>//this file is included in map.c only&#010;</xsl:text>
  <xsl:text>//it is not visible for other source files&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

  <xsl:text>#include "str.h"&#010;</xsl:text>
  
  <xsl:apply-templates/>

  <xsl:text>&#010;</xsl:text>
  <xsl:text>#define __MAP_H&#010;</xsl:text>
  <xsl:text>#endif&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk"/>
<xsl:template match="uta:keySpace"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:dfsFile"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:volatile"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:derivated"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:find"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:rptLayout"></xsl:template>

<xsl:template match="uta:record">
    <xsl:text>static tRecord rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>; //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:if test="@uta:dim!='' or @uta:multi!=''">
        <xsl:text>static tTable tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>;</xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
    <xsl:text>static tContainer cnt</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>;&#010;</xsl:text>

    <xsl:text>static word </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Len[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End)]= {&#010;</xsl:text>

    <xsl:apply-templates mode="fieldLen"/>

    <xsl:text>};</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>static word </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Map[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End)];&#010;</xsl:text>

    <xsl:text>static char </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Fmt[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End)]= {&#010;</xsl:text>

    <xsl:apply-templates mode="fieldFmt"/>

    <xsl:text>};</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:if test="@uta:cache='yes'">
        <xsl:text>static byte </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Cache[&#010;</xsl:text>
        <xsl:apply-templates mode="cache"/>
        <xsl:text>0];&#010;</xsl:text>
    </xsl:if>

    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:table">
    <xsl:text>static tTable tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>; //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>static tContainer cnt</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>;&#010;</xsl:text>
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>static byte </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Cache[</xsl:text>
        <xsl:choose>
            <xsl:when test="@uta:type='ASCII'">
                <xsl:value-of select="@uta:length"/>
            </xsl:when>
            <xsl:when test="@uta:type='binary'">
                <xsl:text>1+</xsl:text>
                <xsl:value-of select="@uta:length"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>sizeof(</xsl:text>
                <xsl:value-of select="@uta:type"/>
                <xsl:text>)</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text>*</xsl:text>
        <xsl:choose>
            <xsl:when test="@uta:dim!=''">
              <xsl:value-of select="@uta:dim"/>
            </xsl:when>
           <xsl:otherwise>
              <xsl:text>DIM(</xsl:text>
              <xsl:value-of select="@uta:id"/>
              <xsl:text>Beg,</xsl:text>
              <xsl:value-of select="@uta:id"/>
              <xsl:text>End)</xsl:text>
           </xsl:otherwise>
        </xsl:choose>
        
        <xsl:text>];&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:search">
    <xsl:text>static tSearch srh</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>; //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:index">
    <xsl:text>static tIndex idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>; //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    
    <xsl:text>static word idxNum</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>[</xsl:text>
    <xsl:value-of select="@uta:dim"/>
    <xsl:text>]; //ordered indices storage&#010;</xsl:text>

    <xsl:if test="@uta:length!=''">
        <xsl:text>static char idxKey</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>[</xsl:text>
        <xsl:value-of select="@uta:dim"/>
        <xsl:text>*</xsl:text>
        <xsl:value-of select="@uta:length"/>
        <xsl:text>]; //key storage&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:volatile">
    <xsl:apply-templates/> <!-- apply generic templates -->
    <xsl:text>typedef struct sVol{&#010;</xsl:text>
    <xsl:apply-templates mode="volatile"/> <!-- a special template to generate volatile buffer -->
    <xsl:text>}tVol;&#010;</xsl:text>
    <xsl:text>static tVol vol; //reserve volatile memory&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:field" mode="fieldLen">
    <xsl:text>    </xsl:text>
    <xsl:variable name="fldLen">
        <xsl:choose>
            <xsl:when test="@uta:type='ASCII'">
                <xsl:value-of select="@uta:length"/>
            </xsl:when>
            <xsl:when test="@uta:type='binary'">
                <xsl:text>1+</xsl:text>
                <xsl:value-of select="@uta:length"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>sizeof(</xsl:text>
                <xsl:value-of select="@uta:type"/>
                <xsl:text>)</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text>, </xsl:text>
    </xsl:variable>
    <xsl:choose>
        <xsl:when test="@uta:repeat!=''">
            <xsl:call-template name="repeat-string">
                <xsl:with-param name="str">
                    <xsl:value-of select="$fldLen"/>
                    <xsl:text>&#010;    </xsl:text>
                </xsl:with-param>
                <xsl:with-param name="cnt" select="@uta:repeat"/>
                <xsl:with-param name="pfx" select="."/>
        	</xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$fldLen"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text> //</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:field" mode="fieldFmt">
    <xsl:text>    </xsl:text>
    <xsl:variable name="fldFmt">
        <xsl:choose>
            <xsl:when test="@uta:type='ASCII'">
                <xsl:text>'A'</xsl:text>
            </xsl:when>
            <xsl:when test="@uta:type='binary'">
                <xsl:text>'B'</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>'N'</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text>, </xsl:text>
    </xsl:variable>
    <xsl:choose>
        <xsl:when test="@uta:repeat!=''">
            <xsl:call-template name="repeat-string">
                <xsl:with-param name="str">
                    <xsl:value-of select="$fldFmt"/>
                    <xsl:text>&#010;    </xsl:text>
                </xsl:with-param>
                <xsl:with-param name="cnt" select="@uta:repeat"/>
                <xsl:with-param name="pfx" select="."/>
        	</xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$fldFmt"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text> //</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:field" mode="cache">
    <xsl:text>    </xsl:text>
    <xsl:choose>
        <xsl:when test="@uta:type='ASCII'">
            <xsl:value-of select="@uta:length"/>
        </xsl:when>
        <xsl:when test="@uta:type='binary'">
            <xsl:text>1+</xsl:text>
            <xsl:value-of select="@uta:length"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>sizeof(</xsl:text>
            <xsl:value-of select="@uta:type"/>
            <xsl:text>)</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="@uta:repeat!=''">
        <xsl:text>*</xsl:text>
        <xsl:value-of select="@uta:repeat"/>
    </xsl:if>
    <xsl:text>+ //</xsl:text>
        <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:field" mode="volatile">
    <xsl:text>    </xsl:text>
    <xsl:choose>
        <xsl:when test="@uta:type='ASCII'">
            <xsl:text>char _</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:text>[</xsl:text>
            <xsl:value-of select="@uta:length"/>
            <xsl:text>]</xsl:text>
            <xsl:if test="parent::node()/@uta:dim">
            	<xsl:text>[</xsl:text>
            	<xsl:value-of select="parent::node()/@uta:dim" />
            	<xsl:text>]</xsl:text>
            </xsl:if>
            <xsl:text>;</xsl:text>
        </xsl:when>
        <xsl:when test="@uta:type='binary'">
            <xsl:text>byte _</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:text>[1+</xsl:text>
            <xsl:value-of select="@uta:length"/>
            <xsl:text>]</xsl:text>
            <xsl:if test="parent::node()/@uta:dim">
            	<xsl:text>[</xsl:text>
            	<xsl:value-of select="parent::node()/@uta:dim" />
            	<xsl:text>]</xsl:text>
            </xsl:if>
            <xsl:text>;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="@uta:type"/>
            <xsl:text> _</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="parent::node()/@uta:dim">
            	<xsl:text>[</xsl:text>
            	<xsl:value-of select="parent::node()/@uta:dim" />
            	<xsl:text>]</xsl:text>
            </xsl:if>
            <xsl:text>;</xsl:text>
        </xsl:otherwise>
        </xsl:choose>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:converters">
    <xsl:text>typedef struct sCvt{&#010;</xsl:text>
    <xsl:text>    word key; //original database key to be converted&#010;</xsl:text>
    <xsl:text>    byte dbf; //original database format&#010;</xsl:text>
    <xsl:text>    byte fmt; //external format&#010;</xsl:text>
    <xsl:text>    word len; //external length&#010;</xsl:text>
    <xsl:text>}tCvt;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <xsl:text>static tCvt cvtTable[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End)]={&#010;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>};&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

        <xsl:text>static tConvert </xsl:text>
    <xsl:value-of select="@uta:id"/>
        <xsl:text>[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
        <xsl:text>End)]; //converters&#010;</xsl:text>
        
    <xsl:text>static tContainer cntCvt[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
        <xsl:text>End)]; //converter containers&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:convert">
    <xsl:text>    {</xsl:text>
    <xsl:value-of select="uta:dbKey"/>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="uta:dbFmt"/>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="uta:fmt"/>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="uta:length"/>
    <xsl:text>}, //</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:if test="@uta:comment!=''">
        <xsl:text>, </xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos">
    <xsl:text>typedef struct sCmb{&#010;</xsl:text>
    <xsl:text>    byte opr; //operation to be performed&#010;</xsl:text>
    <xsl:text>    int arg0; //negative values can be used to transfer arguments by value&#010;</xsl:text>
    <xsl:text>    int arg1;&#010;</xsl:text>
    <xsl:text>    int arg2;&#010;</xsl:text>
    <xsl:text>    word len;&#010;</xsl:text>
    <xsl:text>}tCmb;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>static tCmb cmbTable[DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End)]={&#010;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>};&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
        <xsl:text>static tCombo </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>[DIM(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg,</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>End)]; //combinations&#010;</xsl:text>

        <xsl:text>static tContainer cntCmb[DIM(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg,</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>End)]; //combination containers&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

</xsl:template>

<xsl:template match="uta:combo">
    <xsl:text>    {</xsl:text>
    <xsl:value-of select="uta:operation"/>
    <xsl:text>, </xsl:text>
    <xsl:choose>
        <xsl:when test="uta:arg0/@uta:value!=''">
            <xsl:text>-(int)</xsl:text>
            <xsl:value-of select="uta:arg0/@uta:value"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="uta:arg0"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:choose>
        <xsl:when test="uta:arg1/@uta:value!=''">
            <xsl:text>-(int)</xsl:text>
            <xsl:value-of select="uta:arg1/@uta:value"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="uta:arg1"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:choose>
        <xsl:when test="uta:arg2/@uta:value!=''">
            <xsl:text>-(int)</xsl:text>
            <xsl:value-of select="uta:arg2/@uta:value"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="uta:arg2"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="uta:length"/>
    <xsl:text>}, //</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:if test="@uta:comment!=''">
        <xsl:text>, </xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

</xsl:transform>
