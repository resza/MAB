<?xml version="1.0"?>

<xsl:transform version = "1.0" id="dftc"
    extension-element-prefixes="uta"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
    xmlns:uta = "http://www.ingenico.com/xml/uta"
>

<xsl:include href="inc.xsl"/>

<xsl:template match="/">
    <xsl:text>#include "global.h"&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>#define CHK CHECK(ret>=0,lblKO)&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>/** Get compile date.&#010;</xsl:text>
    <xsl:text> * \return pointer to the date of compilation of the file dft.c&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The function allows to identify the compilation date of the application.&#010;</xsl:text>
    <xsl:text> * It is used together with the function getCmpTim() to detect whether the current version of the data base&#010;</xsl:text>
    <xsl:text> * is created by the current or by the previous version of the application.&#010;</xsl:text>
    <xsl:text> * If the current version of the application and the database are not the same&#010;</xsl:text>
    <xsl:text> * a date base rebuilding is ussually required.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * This method is used while treating POWER ON event.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - getCmpTim()&#010;</xsl:text>
    <xsl:text> * \remark&#010;</xsl:text>
    <xsl:text> * The format of compile date depends on the compiler.&#010;</xsl:text>
    <xsl:text> * It is not the same for Microsoft Visual and ARM.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>const char *getCmpDat(void){ return __DATE__;}&#010;</xsl:text>

    <xsl:text>/** Get compile time.&#010;</xsl:text>
    <xsl:text> * \return pointer to the time of compilation of the file dft.c&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The function allows to identify the compilation time of the application.&#010;</xsl:text>
    <xsl:text> * It is used together with the function getCmpDat() to detect whether the current version of the data base&#010;</xsl:text>
    <xsl:text> * is created by the current or by the previous version of the application.&#010;</xsl:text>
    <xsl:text> * If the current version of the application and the database are not the same&#010;</xsl:text>
    <xsl:text> * a date base rebuilding is ussually required.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * This method is used while treating POWER ON event.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - getCmpDat()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>const char *getCmpTim(void){ return __TIME__;}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates mode="static"/>    
    
    <!-- Build moveSlider() function -->
    <xsl:text>static void moveSlider(char *sld){&#010;</xsl:text>
    <xsl:text>    if(strlen(sld)&gt;=dspW) memset(sld,0,dspW);&#010;</xsl:text>
    <xsl:text>    strcat(sld,".");&#010;</xsl:text>
    <!--<xsl:text>    dspLS(3,sld);&#010;</xsl:text>-->
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <!-- Build dftReset() function -->
    <xsl:text>/** Fill the UTA data base by default values.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of the data structure to be reset to default values.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - 1 if everything is OK&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The default data values are given in uta:dat attributes for fields in single records and table rows.&#010;</xsl:text>
    <xsl:text> * For multirecord fields they are provided in uta:data/uta/instance tags. &#010;</xsl:text>
    <xsl:text> * If the parameter is keyBeg (zero) all data structures are reset to default values.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * It is a time consuming operation.&#010;</xsl:text>
    <xsl:text> * Depending on the number of data elements in the data base it can take up to several minutes.&#010;</xsl:text>
    <xsl:text> * This function is usually called only once just after application download.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int dftReset(word key){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>    char sld[dspW+1];&#010;</xsl:text>
    <xsl:text>    trcFN("dftReset: key=%d\n",key);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    ret= mapReset(key); CHK;&#010;</xsl:text>
    <xsl:text>    if(key==keyBeg){&#010;</xsl:text>
    <xsl:text>        memset(sld,0,dspW+1);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:apply-templates mode="dftResetAll"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>        //reset compiler date and time into reserved fields&#010;</xsl:text>
    <xsl:text>        ret= mapPutStr(appCmpDat,getCmpDat()); CHK;&#010;</xsl:text>
    <xsl:text>        ret= mapPutStr(appCmpTim,getCmpTim()); CHK;&#010;</xsl:text>
    <xsl:text>    }else{&#010;</xsl:text>
    <xsl:text>        switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="dftReset1"/>
    <xsl:text>            default: ret= -1; break;&#010;</xsl:text>
    <xsl:text>        }&#010;</xsl:text>
    <xsl:text>        CHK;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    ret= 1;&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    trcFN("dftReset: ret=%d\n",ret);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk"/>
<xsl:template match="uta:dbk" mode="static"/>
<xsl:template match="uta:keySpace" mode="static"><xsl:apply-templates mode="static"/></xsl:template>
<xsl:template match="uta:volatile" mode="static"></xsl:template>
<xsl:template match="uta:derivated" mode="static"></xsl:template>
<xsl:template match="uta:rptLayout" mode="static"></xsl:template>

<xsl:template match="uta:table" mode="static">
    <xsl:text>static int fillDft</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>(void){&#010;</xsl:text>
    <xsl:choose>
      <xsl:when test="@uta:dim!=''">
        <xsl:text>    return 1;&#010;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>    int ret;&#010;</xsl:text>
        <xsl:text>    word key;&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>    trcS("fillDft</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>: Beg\n");&#010;</xsl:text>
        <xsl:if test="@uta:cache='yes'">
            <xsl:text>    mapCache(</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:text>Beg); //enable cache&#010;</xsl:text>
        </xsl:if>
        <xsl:text>    for(key= </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg+1; key&lt;</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>End; key++){&#010;</xsl:text>
        <xsl:text>        ret= mapPut(key,dft</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>[key-</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg-1],0);&#010;</xsl:text>
        <xsl:text>        CHK;&#010;</xsl:text>
        <xsl:text>    }&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:if test="@uta:cache='yes'">
            <xsl:text>    ret= mapSave(</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:text>Beg); CHECK(ret>=0,lblKO);&#010;</xsl:text>
        </xsl:if>
        <xsl:text>    ret= 1; goto lblEnd;&#010;</xsl:text>
        <xsl:text>lblKO: trcErr(ret); ret= -1;&#010;</xsl:text>
        <xsl:text>lblEnd:&#010;</xsl:text>
        <xsl:if test="@uta:cache='yes'">
            <xsl:text>    mapCache(-</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:text>Beg); //disable cache&#010;</xsl:text>
        </xsl:if>
        <xsl:text>    trcFN("fillDft</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
            <xsl:text>: ret=%d\n",ret);&#010;</xsl:text>
            <xsl:text>    return ret;&#010;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:record" mode="static">
    <xsl:text>static int fillDft</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>(void){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    
    <xsl:choose>
        <xsl:when test="@uta:dim!='' or @uta:multi!=''">
            <xsl:if test="count(uta:data)!=0">
                <xsl:text>    word i;&#010;</xsl:text>
                <xsl:text>    trcS("fillDft</xsl:text>
                <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>: Beg\n");&#010;</xsl:text>
                <xsl:text>&#010;</xsl:text>

                <xsl:if test="@uta:cache='yes'">
                    <xsl:text>    mapCache(</xsl:text>
                    <xsl:value-of select="@uta:id"/>
                    <xsl:text>Beg); //enable cache&#010;</xsl:text>
                </xsl:if>

                <xsl:text>    for(i= 0; i&lt;</xsl:text>
                <xsl:if test="@uta:dim!=''">
                    <xsl:value-of select="@uta:dim"/>
                </xsl:if>
                <xsl:if test="@uta:multi!=''">
                    <xsl:text>DIM(</xsl:text>
                    <xsl:value-of select="@uta:multi"/>
                    <xsl:text>Beg,</xsl:text>
                    <xsl:value-of select="@uta:multi"/>
                    <xsl:text>End)</xsl:text>
                </xsl:if>
                <xsl:text>; i++){&#010;</xsl:text>

                <xsl:if test="@uta:dim!=''">
                    <xsl:text>        if(i>=sizeof(dft</xsl:text>
                    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                    <xsl:value-of select="substring(@uta:id,2)"/>
                    <xsl:text>)/sizeof(t</xsl:text>
                    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                    <xsl:value-of select="substring(@uta:id,2)"/>
                    <xsl:text>Record)) break;&#010;</xsl:text>
                </xsl:if>

                <xsl:text>        ret= mapMove(</xsl:text>
                <xsl:value-of select="@uta:id"/>
                <xsl:text>Beg,i); CHK;&#010;</xsl:text>
                <xsl:apply-templates mode="multiple"/>
                <xsl:if test="@uta:cache='yes'">
                    <xsl:text>        ret= mapSave(</xsl:text>
                    <xsl:value-of select="@uta:id"/>
                    <xsl:text>Beg); CHECK(ret>=0,lblKO);&#010;</xsl:text>
                </xsl:if>
                <xsl:text>    }&#010;</xsl:text>

            </xsl:if>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>    trcS("fillDft</xsl:text>
            <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(@uta:id,2)"/>
            <xsl:text>: Beg\n");&#010;</xsl:text>
            <xsl:text>&#010;</xsl:text>
            <xsl:apply-templates mode="single"/>
        </xsl:otherwise>
    </xsl:choose>
            
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    ret= 1; CHECK(ret>0,lblKO); goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO: ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>    mapCache(-</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg); //disable cache&#010;</xsl:text>
    </xsl:if>
    <xsl:text>    trcFN("fillDft</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>: ret=%d\n",ret);&#010;</xsl:text>
        <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="single"/>
<xsl:template match="uta:field" mode="single">
    <xsl:if test="@uta:data!=''">
        <xsl:choose>
            <xsl:when test="@uta:type='ASCII'">
                <xsl:text>    ret= mapPutStr(</xsl:text>
                <xsl:value-of select="@uta:id"/>
                <xsl:text>,dft</xsl:text>
                <xsl:value-of 
                    select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>); CHK;</xsl:text>
            </xsl:when>
            <xsl:when test="@uta:type='binary'">
                <xsl:text>    ret= mapPut(</xsl:text>
                <xsl:value-of select="@uta:id"/>
                <xsl:text>,dft</xsl:text>
                <xsl:value-of 
                    select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>,(byte)(1+dft</xsl:text>
                <xsl:value-of 
                    select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>[0])); CHK;</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>    ret= mapPut</xsl:text>
                <xsl:value-of 
                    select="translate(substring(@uta:type,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:type,2)"/>
                <xsl:text>(</xsl:text>
                <xsl:value-of select="@uta:id"/>
                <xsl:text>,dft</xsl:text>
                <xsl:value-of 
                    select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                <xsl:value-of select="substring(@uta:id,2)"/>
                <xsl:text>); CHK;</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:dbk" mode="multiple"/>
<xsl:template match="uta:field" mode="multiple">
    <xsl:if test="@uta:repeat!=''">
        <xsl:text>		{word j;&#010;</xsl:text>
        <xsl:text>			for(j= 0; j&lt;</xsl:text>
        <xsl:value-of select="@uta:repeat"/>
        <xsl:text>; j++){&#010;
</xsl:text>
    </xsl:if>
    <xsl:choose>
        <xsl:when test="@uta:type='ASCII'">
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>        </xsl:text>
            </xsl:if>
            <xsl:text>        ret= mapPutStr((word)(</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>+j</xsl:text>
            </xsl:if>
            <xsl:text>),dft</xsl:text>
            <xsl:value-of 
                select="translate(substring(parent::node()/@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(parent::node()/@uta:id,2)"/>
            <xsl:text>[i]._</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>[j]</xsl:text>
            </xsl:if>
            <xsl:text>);</xsl:text>
        </xsl:when>
        <xsl:when test="@uta:type='binary'">
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>        </xsl:text>
            </xsl:if>
            <xsl:text>        ret= mapPut((word)(</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>+j</xsl:text>
            </xsl:if>
            <xsl:text>),dft</xsl:text>
            <xsl:value-of 
                select="translate(substring(parent::node()/@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(parent::node()/@uta:id,2)"/>
            <xsl:text>[i]._</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:text>,(byte)(1+dft</xsl:text>
            <xsl:value-of 
                select="translate(substring(parent::node()/@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(parent::node()/@uta:id,2)"/>
            <xsl:text>[i]._</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>[j]</xsl:text>
            </xsl:if>
            <xsl:text>[0]));</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>        </xsl:text>
            </xsl:if>
            <xsl:text>        ret= mapPut((word)(</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>+j</xsl:text>
            </xsl:if>
            <xsl:text>),&amp;dft</xsl:text>
            <xsl:value-of 
                select="translate(substring(parent::node()/@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(parent::node()/@uta:id,2)"/>
            <xsl:text>[i]._</xsl:text>
            <xsl:value-of select="@uta:id"/>
            <xsl:if test="@uta:repeat!=''">
                <xsl:text>[j]</xsl:text>
            </xsl:if>
            <xsl:text>,sizeof(</xsl:text>
            <xsl:value-of select="@uta:type"/>
            <xsl:text>));</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text> CHK;&#010;</xsl:text>
    <xsl:if test="@uta:repeat!=''">
        <xsl:text>			}&#010;</xsl:text>
        <xsl:text>		}&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:dbk" mode="dftResetAll"/>
<xsl:template match="uta:keySpace" mode="dftResetAll"><xsl:apply-templates mode="dftResetAll"/></xsl:template>
<xsl:template match="uta:volatile" mode="dftResetAll"></xsl:template>
<xsl:template match="uta:derivated" mode="dftResetAll"></xsl:template>
<xsl:template match="uta:rptLayout" mode="dftResetAll"></xsl:template>

<xsl:template match = "uta:table|uta:record" mode="dftResetAll">
    <xsl:choose>
        <xsl:when test="../@uta:reset='no'"/>
        <xsl:otherwise>
          <xsl:text>        moveSlider(sld); ret= fillDft</xsl:text>
          <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
          <xsl:value-of select="substring(@uta:id,2)"/>
          <xsl:text>(); CHK;</xsl:text>
          <xsl:text>&#010;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="uta:dbk" mode="dftReset1"/>
<xsl:template match="uta:keySpace" mode="dftReset1"><xsl:apply-templates mode="dftReset1"/></xsl:template>
<xsl:template match="uta:volatile" mode="dftReset1"></xsl:template>
<xsl:template match="uta:derivated" mode="dftReset1"></xsl:template>
<xsl:template match="uta:rptLayout" mode="dftReset1"></xsl:template>

<xsl:template match = "uta:table|uta:record" mode="dftReset1">
  <xsl:text>            case </xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>Beg: ret= fillDft</xsl:text>
  <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
  <xsl:value-of select="substring(@uta:id,2)"/>
  <xsl:text>(); break;</xsl:text>
  <xsl:text>&#010;</xsl:text>
</xsl:template>

</xsl:transform>
