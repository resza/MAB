<?xml version="1.0"?>

<xsl:transform version = "1.0" id="scnc"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

<xsl:include href="inc.xsl"/>
<xsl:key name="shape" match="uta:shape" use="@uta:id" /> 
<xsl:key name="drawing" match="uta:drawing" use="@uta:id" /> 

<xsl:template name="fgd-color">
  <xsl:param name="color"/>
  <xsl:choose>
    <xsl:when test="$color!=''">
      <xsl:value-of select="$color"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>clrFgd</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="/">
  <xsl:text>#include &lt;string.h&gt;&#010;</xsl:text>
  <xsl:text>#include "str.h"&#010;</xsl:text>
  <xsl:text>#include "bmp.h"&#010;</xsl:text>
  <xsl:text>#include "scn.h"&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

  <xsl:text>//These extern functions are declared in map.c&#010;</xsl:text>
  <xsl:text>extern const tContainer *getContainer(word key);&#010;</xsl:text>    
  <xsl:text>extern word relKey(word key);&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

  <xsl:text>//Calculate relative position within _shapes array&#010;</xsl:text>
  <xsl:text>#define RELSCN(KEY) (KEY-scnBeg-1)&#010;</xsl:text>    
  <xsl:text>#define RELGUI(KEY) (KEY-guiBeg-1)&#010;</xsl:text>    
  <xsl:text>&#010;</xsl:text>

  <xsl:apply-templates/>

  <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk"/>
<xsl:template match="uta:keySpace"/>
<xsl:template match="uta:enum"/>

<xsl:template match="uta:scenes">
  <!-- build all static functions -->
  <xsl:text>static int scnTextInit(tText *shp,word msg,byte fnt,card clr){&#010;</xsl:text>
  <xsl:text>    const tContainer *cnt;&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

  <xsl:text>    cnt= getContainer(msg);&#010;</xsl:text>
  <xsl:text>    switch(cnt->typ){&#010;</xsl:text>
  <xsl:text>      case 't': case 'r': msg= relKey(msg); break;&#010;</xsl:text>
  <xsl:text>      default: msg= NULLKEY; break;&#010;</xsl:text>
  <xsl:text>    }&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

  <xsl:text>    return drwTextInit(shp,cnt,msg,fnt,clr);&#010;</xsl:text>
  <xsl:text>}&#010;</xsl:text>
  <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates/>
    <xsl:text>&#010;</xsl:text>

    <!-- build scnInit function -->
    <xsl:text>/** Initialize all shapes and drawings.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - number of shapes and drawings initialized&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> */ &#010;</xsl:text>
    <xsl:text>int scnInit(void){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates mode="scnInit"/>

    <xsl:if test="count(uta:gui/*)!=0">
        <xsl:text>    ret= guiInit(); </xsl:text>
        <xsl:text>CHECK(ret&gt;=0,lblKO);&#010;</xsl:text>
    </xsl:if>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>    ret= 1;&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    trcFN("scnInit: ret=%d\n",ret);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- build scnDraw function -->
    <xsl:text>/** Render a drawing on canvas.&#010;</xsl:text>
    <xsl:text> * \param scn (I) scene key to render&#010;</xsl:text>
    <xsl:text> * \param x (I) horizontal (left-to-right) coordinate reference point&#010;</xsl:text>
    <xsl:text> * \param y (I) vertical (top-to bottom) coordinate reference point&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - number of shapes drawn&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the scn should reference a drawing or shape&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int scnDraw(word scn,word x, word y){&#010;</xsl:text>
    <xsl:text>    return drwShapeDraw(&amp;_shapes[RELSCN(scn)],x,y);&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- build scnPos function -->
    <xsl:text>/** Move composite shape (video or slider) to a given position.&#010;</xsl:text>
    <xsl:text> * \param scn (I) scene key to modify&#010;</xsl:text>
    <xsl:text> * \param pos (I) position to change to&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the current position &#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the scn should reference a composite shape with position treatment&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int scnPos(word scn,word pos){&#010;</xsl:text>
    <xsl:text>    byte typ;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>    typ= _shapes[RELSCN(scn)].typ;&#010;</xsl:text>
    <xsl:text>     switch(typ){&#010;</xsl:text>
    <xsl:text>        case shpVideo: return vidPos(_shapes[RELSCN(scn)].ptr.vid,pos);&#010;</xsl:text>
    <xsl:text>        case shpSlider: return sldPos(_shapes[RELSCN(scn)].ptr.sld,pos);&#010;</xsl:text>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:if test="count(uta:gui/*)!=0">
      <!-- build guiDraw function -->
      <xsl:text>/** Wait for the user input&#010;</xsl:text>
      <xsl:text> * \param gui (I) GUI scene to be displayed&#010;</xsl:text>
      <xsl:text> * \return non-negative if OK; negative if failure&#010;</xsl:text>
      <xsl:text> * \sa guiWait&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int guiDraw(word gui){&#010;</xsl:text>
      <xsl:text>    return drwDrawingDraw(_gui[RELGUI(gui)].drw,_gui[RELGUI(gui)].x,_gui[RELGUI(gui)].y);&#010;</xsl:text>
      <xsl:text>}&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>

      <!-- build guiWait function -->
      <xsl:text>/** Wait for the user input&#010;</xsl:text>
      <xsl:text> * \param gui (I) GUI scene to be displayed while waiting&#010;</xsl:text>
      <xsl:text> * \param buf (O) input buffer that will be filled depending on entry mode used&#010;</xsl:text>
      <xsl:text> * \return entry mode&#010;</xsl:text>
      <xsl:text> *  - 'k','K': keyboard&#010;</xsl:text>
      <xsl:text> *  - 't','T': touch screen&#010;</xsl:text>
      <xsl:text> *  - 'm': magnetic stripe card swipe&#010;</xsl:text>
      <xsl:text> *  - 'c': chip card insertion&#010;</xsl:text>
      <xsl:text> *  - 'b': bar code scanning&#010;</xsl:text>
      <xsl:text> * - negative if failure&#010;</xsl:text>
      <xsl:text> * \sa guiInputWait&#010;</xsl:text>
      <xsl:text> * \sa guiWaitDly&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int guiWait(word gui,tBuffer *buf){&#010;</xsl:text>
      <xsl:text>    return guiInputWait(&amp;_gui[RELGUI(gui)],1,buf);&#010;</xsl:text>
      <xsl:text>}&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>

      <!-- build guiWaitDly function -->
      <xsl:text>/** Wait for the user input with a given timeout&#010;</xsl:text>
      <xsl:text> * \param gui (I) GUI scene to be displayed while waiting&#010;</xsl:text>
      <xsl:text> * \param buf (O) input buffer that will be filled depending on entry mode used&#010;</xsl:text>
      <xsl:text> * \param dly (I) waiting timeout&#010;</xsl:text>
      <xsl:text> * \return entry mode&#010;</xsl:text>
      <xsl:text> *  - 'k','K': keyboard&#010;</xsl:text>
      <xsl:text> *  - 't','T': touch screen&#010;</xsl:text>
      <xsl:text> *  - 'm': magnetic stripe card swipe&#010;</xsl:text>
      <xsl:text> *  - 'c': chip card insertion&#010;</xsl:text>
      <xsl:text> *  - 'b': bar code scanning&#010;</xsl:text>
      <xsl:text> * - negative if failure&#010;</xsl:text>
      <xsl:text> * \sa guiInputWait&#010;</xsl:text>
      <xsl:text> * \sa guiWait&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int guiWaitDly(word gui,tBuffer *buf,byte dly){&#010;</xsl:text>
      <xsl:text>    tInput *inp;&#010;</xsl:text>
      <xsl:text>    inp= &amp;_gui[RELGUI(gui)];&#010;</xsl:text>
      <xsl:text>    inp->dly= dly;&#010;</xsl:text>
      <xsl:text>    return guiInputWait(inp,1,buf);&#010;</xsl:text>
      <xsl:text>}&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>

      <!-- build guiDialog function -->
      <xsl:text>/** Wait for the user input&#010;</xsl:text>
      <xsl:text> * \param gui (I) background GUI scene&#010;</xsl:text>
      <xsl:text> * \param key (I) input data element&#010;</xsl:text>
      <xsl:text> * \param spc (I) special characters&#010;</xsl:text>
      <xsl:text> * \param maxLen (I) maximal input length accepted (if 0, the length of key is used)&#010;</xsl:text>
      <xsl:text> * \return last key pressed or 0 in case of timeout; negative if failure&#010;</xsl:text>
      <xsl:text> * \sa guiInputDialog&#010;</xsl:text>
      <xsl:text> *&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int guiDialog(word gui,word key,const char *spc,byte maxLen){&#010;</xsl:text>
      <xsl:text>    const tContainer *cnt;&#010;</xsl:text>
      <xsl:text>    cnt= getContainer(key);&#010;</xsl:text>
      <xsl:text>    key= relKey(key);&#010;</xsl:text>
      <xsl:text>    return guiInputDialog(&amp;_gui[RELGUI(gui)],cnt,key,spc,maxLen);&#010;</xsl:text>
      <xsl:text>}&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>

      <!-- build guiSelect function -->
      <xsl:text>/** Wait for the user input&#010;</xsl:text>
      <xsl:text> * \param gui (I) GUI selector scene&#010;</xsl:text>
      <xsl:text> * \param str (I) zero-ended list of items&#010;</xsl:text>
      <xsl:text> * \param pos (I) initial selector position, WORDHL(top,foc)&#010;</xsl:text>
      <xsl:text> * \return result selector position; special return codes start by 0xFF&#010;</xsl:text>
      <xsl:text> * \sa guiInputSelect&#010;</xsl:text>
      <xsl:text> *&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int guiSelect(word gui,const Pchar * str, word pos){&#010;</xsl:text>
      <xsl:text>    return guiInputSelect(&amp;_gui[RELGUI(gui)],str,pos);&#010;</xsl:text>
      <xsl:text>}&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>

      <!-- build guiBrowse function -->
      <xsl:text>/** Wait for the user input&#010;</xsl:text>
      <xsl:text> * \param gui (I) GUI selector scene&#010;</xsl:text>
      <xsl:text> * \param brw (I) virtual browser methods table&#010;</xsl:text>
      <xsl:text> * \param pos (I) initial selector position, CARDHL(top,foc)&#010;</xsl:text>
      <xsl:text> * \return result browser position or 0 in case of timeout or abort&#010;</xsl:text>
      <xsl:text> * \sa guiInputBrowse&#010;</xsl:text>
      <xsl:text> *&#010;</xsl:text>
      <xsl:text> */&#010;</xsl:text>
      <xsl:text>int guiBrowse(word gui,const tVmtBrw * brw, card pos){&#010;</xsl:text>
      <xsl:text>    return guiInputBrowse(&amp;_gui[RELGUI(gui)],brw,pos);&#010;</xsl:text>
      <xsl:text>}&#010;</xsl:text>
      <xsl:text>&#010;</xsl:text>

    </xsl:if>

    <!-- build static scnShowHide function -->
    <xsl:text>static void scnShowHide(word scn,byte idx,char mod){&#010;</xsl:text>
    <xsl:text>    byte dim;&#010;</xsl:text>
    <xsl:text>    tDrawing *drw;&#010;</xsl:text>
    <xsl:text>    scn= RELSCN(scn);&#010;</xsl:text>
    <xsl:text>    drw= _shapes[scn].ptr.drw;&#010;</xsl:text>
    <xsl:text>    dim= drwDim(drw);&#010;</xsl:text>
    <xsl:text>    switch(mod){&#010;</xsl:text>
    <xsl:text>        case 'h':&#010;</xsl:text>
    <xsl:text>          if(idx&lt;dim) drwHide(drw,idx);&#010;</xsl:text>
    <xsl:text>          else while(dim--) drwHide(drw,dim);&#010;</xsl:text>
    <xsl:text>          break;&#010;</xsl:text>
    <xsl:text>        case 's':&#010;</xsl:text>
    <xsl:text>          if(idx&lt;dim) drwShow(drw,idx);&#010;</xsl:text>
    <xsl:text>          else while(dim--) drwShow(drw,dim);&#010;</xsl:text>
    <xsl:text>          break;&#010;</xsl:text>
    <xsl:text>        default:&#010;</xsl:text>
    <xsl:text>          break;&#010;</xsl:text>
    <xsl:text>     }&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>


    <!-- build scnHide function -->
    <xsl:text>/** Hide a shape within a drawing.&#010;</xsl:text>
    <xsl:text> * \param drw (I) drawing key&#010;</xsl:text>
    <xsl:text> * \param idx (I) index of the shape within the drawing&#010;</xsl:text>
    <xsl:text> * If idx==0xFF all shapes in the drawing are hidden&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the scn should reference a drawing shape&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>void scnHide(word drw,byte idx){&#010;</xsl:text>
    <xsl:text>    scnShowHide(drw,idx,'h');&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- build scnShow function -->
    <xsl:text>/** Show a shape within a drawing.&#010;</xsl:text>
    <xsl:text> * \param drw (I) drawing key&#010;</xsl:text>
    <xsl:text> * \param idx (I) index of the shape within the drawing&#010;</xsl:text>
    <xsl:text> * If idx==0xFF all shapes in the drawing are shown&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the scn should reference a drawing shape&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>void scnShow(word drw,byte idx){&#010;</xsl:text>
    <xsl:text>    scnShowHide(drw,idx,'s');&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

</xsl:template>

<xsl:template match="uta:gui">
    <xsl:apply-templates/>
    <xsl:text>&#010;</xsl:text>

    <!-- build guiInit function -->
    <xsl:text>/** Initialize all gui elements.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - number of gui elements initialized&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> */ &#010;</xsl:text>
    <xsl:text>int guiInit(void){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates mode="guiInit"/>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>    ret= 1;&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    trcFN("guiInit: ret=%d\n",ret);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

</xsl:template>

<xsl:template match="uta:input" mode="guiInit">
    <xsl:if test="count(uta:virtual)!='0'">
      <xsl:text>    memset(&amp;</xsl:text>
      <xsl:text>_vmt</xsl:text>
      <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
      <xsl:value-of select="substring(@uta:id,2)"/>
      <xsl:text>,0,sizeof(tVmtGui));&#010;</xsl:text>
      <xsl:apply-templates mode="vmtInit"/>
    </xsl:if>
    <xsl:text>    ret= guiInputInit(&amp;_gui[</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>-guiBeg-1], &amp;_</xsl:text>
    <xsl:value-of select="@uta:guiDrawing"/>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="@uta:rfpX"/>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="@uta:rfpY"/>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="@uta:entry"/>
    <xsl:text>, </xsl:text>
    <xsl:choose>
      <xsl:when test="count(uta:virtual)!='0'">
        <xsl:text>&amp;</xsl:text>
      <xsl:text>_vmt</xsl:text>
      <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
      <xsl:value-of select="substring(@uta:id,2)"/>
      </xsl:when>
      <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:value-of select="@uta:delay"/>
    <xsl:text>);&#010;</xsl:text>
    <xsl:text>    CHECK(ret&gt;=0,lblKO);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:virtual" mode="vmtInit">
    <xsl:text>    _vmt</xsl:text>
    <xsl:value-of select="translate(substring(../@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(../@uta:id,2)"/>
    <xsl:text>.</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>= </xsl:text>
    <xsl:value-of select="@uta:call"/>
    <xsl:text>;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:scene">
    <xsl:apply-templates/>

    <xsl:text>static int scnInit</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>(void){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:apply-templates mode="scnInit"/>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>    ret= 1;&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    trcFN("scnInit</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>: ret=%d\n",ret);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:scene" mode="scnInit">
    <xsl:text>    ret= scnInit</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>(); CHECK(ret&gt;=0,lblKO);</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:drawing" mode="scnInit">
  <xsl:text>    ret= init</xsl:text>
  <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
  <xsl:value-of select="substring(@uta:id,2)"/>
  <xsl:text>(); CHECK(ret&gt;=0,lblKO);&#010;</xsl:text>

  <xsl:text>    ret= drwShapeInit(&amp;_shapes[RELSCN(</xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>)],shpDrawing,&amp;_</xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>);</xsl:text>
  <xsl:text> CHECK(ret&gt;=0,lblKO);&#010;</xsl:text>

</xsl:template>

<xsl:template match="uta:shape" mode="scnInit">
  <xsl:choose>
    <xsl:when test="@uta:type='shpPixel'">
      <xsl:text>    ret= drwPixelInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <!-- TBD
    <xsl:when test="@uta:type='shpLine'">
      <xsl:text>    ret= drwLineInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:hor"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:ver"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    -->
    <xsl:when test="@uta:type='shpBar'">
      <xsl:text>    ret= drwBarInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:width"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:height"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpText'">
      <xsl:text>    ret= scnTextInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:msg"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:font"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpTile'">
      <xsl:text>    ret= drwTileInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:bitmap!=''"><xsl:value-of select="@uta:bitmap"/></xsl:when>
        <xsl:when test="@uta:pic!=''">
          <xsl:text>picBmpPtr(</xsl:text>
          <xsl:text>pic</xsl:text>
          <xsl:value-of select="translate(substring(@uta:pic,1,1),$az,$AZ)"/>
          <xsl:value-of select="substring(@uta:pic,2,string-length(@uta:pic)-5)"/>
          <xsl:text>)</xsl:text>
        </xsl:when>
      </xsl:choose>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:format='bmp'"><xsl:text>0</xsl:text></xsl:when>
        <xsl:otherwise><xsl:value-of select="@uta:width"/></xsl:otherwise>
      </xsl:choose>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:format='bmp'"><xsl:text>0</xsl:text></xsl:when>
        <xsl:otherwise><xsl:value-of select="@uta:height"/></xsl:otherwise>
      </xsl:choose>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:format='bmp'"><xsl:text>-255</xsl:text></xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="fgd-color">
            <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpBox'">
      <xsl:text>    ret= drwBoxInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:width"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:height"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpCircle'">
      <xsl:text>    ret= drwCircleInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:radius"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpDisc'">
      <xsl:text>    ret= drwDiscInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:radius"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <!-- TBD
    <xsl:when test="@uta:type='shpBezier'">
      <xsl:text>    ret= scnBezierInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:call-template name="fgd-color">
        <xsl:with-param name="color"><xsl:value-of select="@uta:color"/></xsl:with-param>
      </xsl:call-template>
      <xsl:text>);</xsl:text>
    </xsl:when>
    -->
    <xsl:when test="@uta:type='shpButton'">
      <xsl:text>    ret= drwButtonInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:text>&amp;_</xsl:text>
      <xsl:value-of select="@uta:box"/>
      <xsl:text>,</xsl:text>
      <xsl:text>CARDHL(</xsl:text>
      <xsl:text>WORDHL(</xsl:text>
	      <xsl:choose>
	      	<xsl:when test="@uta:horTxt!=''">
	      		<xsl:value-of select="@uta:horTxt"/>
	      	</xsl:when>
	      	<xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
	      </xsl:choose>
	     <xsl:text>,</xsl:text>
	      <xsl:choose>
	      	<xsl:when test="@uta:verTxt!=''">
	      		<xsl:value-of select="@uta:verTxt"/>
	      	</xsl:when>
	      	<xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
	      </xsl:choose>
	     <xsl:text>)</xsl:text>
	     <xsl:text>,</xsl:text>
	     <xsl:text>WORDHL(</xsl:text>
	      <xsl:choose>
	      	<xsl:when test="@uta:horIco!=''">
	      		<xsl:value-of select="@uta:horIco"/>
	      	</xsl:when>
	      	<xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
	      </xsl:choose>
	     <xsl:text>,</xsl:text>
	      <xsl:choose>
	      	<xsl:when test="@uta:verIco!=''">
	      		<xsl:value-of select="@uta:verIco"/>
	      	</xsl:when>
	      	<xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
	      </xsl:choose>
	     <xsl:text>)</xsl:text>
      <xsl:text>)</xsl:text>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:txt!=''">
          <xsl:text>&amp;_</xsl:text>
          <xsl:value-of select="@uta:txt"/>
        </xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
      </xsl:choose>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:ico!=''">
          <xsl:text>&amp;_</xsl:text>
          <xsl:value-of select="@uta:ico"/>
        </xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
      </xsl:choose>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:palette!=''"><xsl:value-of select="@uta:palette"/></xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
      </xsl:choose>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpSlider'">
      <xsl:text>    ret= drwSliderInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:text>&amp;_</xsl:text>
      <xsl:value-of select="@uta:beg"/>
      <xsl:text>,</xsl:text>
      <xsl:text>&amp;_</xsl:text>
      <xsl:value-of select="@uta:end"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:dx"/>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="@uta:dy"/>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpSelector'">
      <xsl:text>    ret= drwSelectorInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:text>&amp;_</xsl:text>
      <xsl:value-of select="@uta:bgd"/>
      <xsl:text>,</xsl:text>
      <xsl:text>&amp;_</xsl:text>
      <xsl:value-of select="@uta:foc"/>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:slider!=''">
          <xsl:text>&amp;_</xsl:text>
          <xsl:value-of select="@uta:slider"/>
        </xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
      </xsl:choose>
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="@uta:palette!=''"><xsl:value-of select="@uta:palette"/></xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
      </xsl:choose>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:when test="@uta:type='shpVideo'">
      <xsl:text>    ret= drwVideoInit(&amp;_</xsl:text>
      <xsl:value-of select="@uta:id"/>
      <xsl:text>,</xsl:text>
      <xsl:text>&amp;_</xsl:text>
      <xsl:value-of select="@uta:drawing"/>
      <xsl:text>);</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>UNDER CONSTRUCTION</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text> CHECK(ret&gt;=0,lblKO);&#010;</xsl:text>

  <xsl:text>    ret= drwShapeInit(&amp;_shapes[RELSCN(</xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>)],</xsl:text>
  <xsl:value-of select="@uta:type"/>
  <xsl:text>,&amp;_</xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>);</xsl:text>
  <xsl:text> CHECK(ret&gt;=0,lblKO);&#010;</xsl:text>

  <xsl:if test="@uta:kbd!=''">
    <xsl:text>    drwShapeKey(&amp;_shapes[RELSCN(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>)],</xsl:text>
    <xsl:value-of select="@uta:kbd"/>
    <xsl:text>);&#010;</xsl:text>
  </xsl:if>
  <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:drawing">
    <xsl:text>static int init</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>(void){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>    byte idx;&#010;</xsl:text>

    <xsl:apply-templates mode="initDrwDcl"/>

    <xsl:text>    static tShape shp[</xsl:text>
    <xsl:value-of select="count(uta:drawingShape)"/>
    <xsl:text>+1]; //one more sentinel shape (zero pointer)&#010;</xsl:text>
    <xsl:text>    static card rfp[</xsl:text>
    <xsl:value-of select="count(uta:drawingShape)"/>
    <xsl:text>];&#010;</xsl:text>

    <xsl:text>&#010;</xsl:text>
    <xsl:text>    memset(shp,0,sizeof(shp));&#010;</xsl:text>
        <xsl:text>    memset(rfp,0,sizeof(rfp));&#010;</xsl:text>
        <xsl:text>    idx= 0;&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>

        <xsl:apply-templates mode="initDrw"/>

    <xsl:text>    ret= drwDrawingInit(&amp;_</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>,shp,rfp); //initialize drawing scene&#010;</xsl:text>
    <xsl:text>    CHECK(ret>=0,lblKO);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>    ret= idx;&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:enum" mode="initDrwDcl">
    <xsl:text>    enum{ //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:apply-templates mode="initDrwDcl"/>
    <xsl:text>    eEnd&#010;</xsl:text>
    <xsl:text>    };&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:enumKey"  mode="initDrwDcl">
    <xsl:text>        </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:if test="@uta:data!=''">
        <xsl:text>=</xsl:text>
        <xsl:value-of select="@uta:data"/>
    </xsl:if>
    <xsl:text>,</xsl:text>
    <xsl:if test="@uta:comment!=''">
        <xsl:text> //</xsl:text>
        <xsl:value-of select="@uta:comment"/>
    </xsl:if>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

  <xsl:template match="uta:drawingShape" mode="initDrw">
    <xsl:text>    ret= drwShapeInit(&amp;shp[idx],</xsl:text>
    <xsl:choose>
      <xsl:when test="key('shape',@uta:ref)/@uta:type!=''">
        <xsl:value-of select="key('shape',@uta:ref)/@uta:type"/>
      </xsl:when>
      <xsl:otherwise><xsl:text>shpDrawing</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>,&amp;_</xsl:text>
    <xsl:value-of select="@uta:ref"/>
    <xsl:text>); CHECK(ret&gt;=0,lblKO);</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <xsl:if test="key('shape',@uta:ref)/@uta:kbd!=''">
      <xsl:text>    drwShapeKey(&amp;shp[idx],</xsl:text>
      <xsl:value-of select="key('shape',@uta:ref)/@uta:kbd"/>
      <xsl:text>);&#010;</xsl:text>
    </xsl:if>

    <xsl:text>    rfp[idx++]= CARDHL(</xsl:text>
    <xsl:value-of select="@uta:rfpX"/>
    <xsl:text>,</xsl:text>
    <xsl:value-of select="@uta:rfpY"/>
    <xsl:text>);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
  </xsl:template>

  <xsl:template match="uta:rptLayout"/>
</xsl:transform>
