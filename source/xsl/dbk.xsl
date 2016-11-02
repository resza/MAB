<?xml version="1.0"?>

<xsl:transform version = "1.0" id="dbk"
    extension-element-prefixes="uta"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:uta="http://www.ingenico.com/xml/uta"
>

  <xsl:output
    method="xml"
    encoding="ISO-8859-1"
    doctype-public="-//OASIS//DTD DocBook XML V4.51b//EN"
    doctype-system="http://www.oasis-open.org/docbook/xml/4.5b1/docbookx.dtd"
    indent="yes" />
  <xsl:strip-space elements="*"/>

  <xsl:key name="enumval" match="uta:enumKey" use="@uta:id" /> 
  <xsl:key name="defineval" match="uta:define" use="@uta:id" /> 
  <xsl:key name="recordid" match="uta:record" use="@uta:id" /> 
  <xsl:key name="tableid" match="uta:table" use="@uta:id" /> 
  <xsl:key name="fieldid" match="uta:field" use="@uta:id" /> 
  <xsl:key name="rowid" match="uta:row" use="@uta:id" /> 
  <xsl:key name="convertid" match="uta:convert" use="@uta:id" /> 
  <xsl:key name="comboid" match="uta:combo" use="@uta:id" /> 
  <xsl:key name="sectionid" match="uta:section" use="@uta:id" /> 

  <xsl:template name="findval">
    <xsl:param name="len"/>
      <xsl:if test="$len!=''">
          <xsl:choose>
              <xsl:when test="
                key('defineval',$len)/@uta:data!='' 
                or key('enumval',$len)/@uta:data!=''
                ">  
                  <xsl:element name="xref">
                      <xsl:attribute name="linkend"><xsl:value-of select="$len"/></xsl:attribute>
                      <xsl:attribute name="endterm"><xsl:value-of select="$len"/></xsl:attribute>
                  </xsl:element>
              </xsl:when>
              <xsl:otherwise>
                  <xsl:value-of select="$len"/>
              </xsl:otherwise>
          </xsl:choose>                    
      </xsl:if>
  </xsl:template>

  <xsl:template name="findid">
    <xsl:param name="id"/>
      <xsl:if test="$id!=''">
          <xsl:choose>
              <xsl:when test="
                key('recordid',$id)/@uta:id!=''
                or key('tableid',$id)/@uta:id!=''
                or key('fieldid',$id)/@uta:id!=''
                or key('rowid',$id)/@uta:id!=''
                or key('convertid',$id)/@uta:id!=''
                or key('comboid',$id)/@uta:id!=''
                or key('sectionid',$id)/@uta:id!=''
                ">  
                  <xsl:element name="xref">
                      <xsl:attribute name="linkend"><xsl:value-of select="$id"/></xsl:attribute>
                  </xsl:element>
              </xsl:when>
              <xsl:otherwise>
                  <xsl:value-of select="$id"/>
              </xsl:otherwise>
          </xsl:choose>                    
      </xsl:if>
  </xsl:template>

    <xsl:template name="anchor">
      <xsl:param name="id"/>
      <xsl:choose>
          <xsl:when test="name(../../.)!='uta:drawing'">
            <xsl:element name="para">
              <xsl:attribute name="id"><xsl:value-of select="$id"/></xsl:attribute>
              <xsl:attribute name="xreflabel"><xsl:value-of select="@uta:id"/></xsl:attribute>
              <xsl:value-of select="$id"/>
            </xsl:element>
        </xsl:when>
        <xsl:otherwise>
            <xsl:element name="para"><xsl:value-of select="$id"/></xsl:element>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:template>

  <xsl:template match="/">
    <xsl:comment>This file is generated from dbs.xml by dbk.xsl</xsl:comment>
    <xsl:element name="article">
      <xsl:text>&#x000A;  </xsl:text>
        <xsl:element name="title">
          <xsl:text>UTA Application Data Base Structure</xsl:text>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;    </xsl:text>

      <xsl:apply-templates/>

    </xsl:element>
    <xsl:text>&#x000A;</xsl:text>
  </xsl:template>

  <xsl:template match="uta:dbk">
    <xsl:text>&#x000A;</xsl:text>
    <xsl:comment>Start of dbk content</xsl:comment>
    <xsl:text>&#x000A;</xsl:text>
    <xsl:copy-of select='./*'/>
    <xsl:text>&#x000A;</xsl:text>
    <xsl:comment>End of dbk content</xsl:comment>
    <xsl:text>&#x000A;</xsl:text>
    <xsl:text>&#x000A;</xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:enum">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Enum </xsl:text>
        <xsl:value-of select="@uta:id"/>
      </xsl:element> <!-- title -->
      <xsl:text>&#x000A;    </xsl:text>
      
      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Enum </xsl:text>
          <xsl:value-of select="@uta:comment"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>3</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Data</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Comment</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Data</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Comment</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
  
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;    </xsl:text>
    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;  </xsl:text>
  </xsl:template>

  <xsl:template match="uta:enumKey">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="anchor">
          <xsl:with-param name="id"><xsl:value-of select="@uta:id"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="align"><xsl:text>right</xsl:text></xsl:attribute>
        <xsl:choose>
            <xsl:when test="@uta:data!=''"><xsl:value-of select="@uta:data"/></xsl:when>
            <xsl:otherwise><xsl:value-of select="position()-1"/></xsl:otherwise>
        </xsl:choose>                    
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:value-of select="@uta:comment"/>
      </xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element>
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:def">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Defines</xsl:text>
      </xsl:element> <!-- title -->
      <xsl:text>&#x000A;    </xsl:text>

      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Enum </xsl:text>
          <xsl:value-of select="@uta:comment"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>3</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Data</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Comment</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
  
          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Data</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Comment</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>
  
          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
  
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;    </xsl:text>
    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;    </xsl:text>
  </xsl:template>

  <xsl:template match="uta:define">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="anchor">
          <xsl:with-param name="id"><xsl:value-of select="@uta:id"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="align"><xsl:text>right</xsl:text></xsl:attribute>
        <xsl:value-of select="@uta:data"/>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:value-of select="@uta:comment"/>
      </xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element>
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:keySpace">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Key Space </xsl:text>
      </xsl:element>
      <xsl:text>&#x000A;    </xsl:text>
      <xsl:apply-templates/>
    </xsl:element>
    <xsl:text>&#x000A;    </xsl:text>
  </xsl:template>

  <xsl:template match="uta:dfsFile">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>DFS file </xsl:text>
        <xsl:value-of select="@uta:id"/>
      </xsl:element>
      <xsl:text>&#x000A;      </xsl:text>
      <xsl:apply-templates/>
    </xsl:element>
    <xsl:text>&#x000A;    </xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:volatile">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Volatile memory</xsl:text>
      </xsl:element>
      <xsl:text>&#x000A;      </xsl:text>
      <xsl:apply-templates/>
    </xsl:element>
    <xsl:text>&#x000A;    </xsl:text>
  </xsl:template>

  <xsl:template match="uta:derivated">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Derivated elements</xsl:text>
      </xsl:element>
      <xsl:text>&#x000A;      </xsl:text>
      <xsl:apply-templates/>
    </xsl:element>
    <xsl:text>&#x000A;    </xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:record">
    <xsl:element name="section">
      <xsl:attribute name="id"><xsl:value-of select="@uta:id"/></xsl:attribute>
      <xsl:attribute name="xreflabel"><xsl:value-of select="@uta:id"/></xsl:attribute>
      <xsl:element name="title">
        <xsl:text>Record </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>: </xsl:text>
        <xsl:value-of select="@uta:comment"/>
      </xsl:element> <!-- section -->
      <xsl:text>&#x000A;      </xsl:text>

      <xsl:element name="para">
        <xsl:text>Settings: </xsl:text>
        <xsl:text>&#x000A;        </xsl:text>
        <xsl:element name="itemizedlist">
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:element name="listitem">
            <xsl:text>Number of fields in the record: </xsl:text>
            <xsl:value-of select="count(uta:field)"/>
          </xsl:element> <!-- listitem -->
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:element name="listitem">
            <xsl:text>Cache: </xsl:text>
              <xsl:choose>
                  <xsl:when test="@uta:cache='yes'"><xsl:text>Enabled</xsl:text></xsl:when>
                  <xsl:otherwise><xsl:text>Disabled</xsl:text></xsl:otherwise>
              </xsl:choose>                    
          </xsl:element> <!-- listitem -->
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:if test="@uta:dim!=''">
            <xsl:element name="listitem">
              <xsl:text>Number of rows in multirecord: </xsl:text>
              <xsl:call-template name="findval">
                <xsl:with-param name="len"><xsl:value-of select="@uta:dim"/></xsl:with-param>
              </xsl:call-template>
            </xsl:element> <!-- listitem -->
            <xsl:text>&#x000A;          </xsl:text>
          </xsl:if>

          <xsl:if test="@uta:multi!=''">
            <xsl:element name="listitem">
              <xsl:text>Number of rows in multirecord: </xsl:text>
              <xsl:value-of select="count(uta:data/uta:instance)"/>
            </xsl:element> <!-- listitem -->
            <xsl:text>&#x000A;          </xsl:text>

            <xsl:element name="listitem">
              <xsl:text>Row prefix: </xsl:text>
              <xsl:value-of select="@uta:multi"/>
            </xsl:element> <!-- listitem -->
            <xsl:text>&#x000A;          </xsl:text>
          </xsl:if>

        </xsl:element> <!-- itemizedlist-->
        <xsl:text>&#x000A;        </xsl:text>
      </xsl:element> <!-- para -->
      <xsl:text>&#x000A;      </xsl:text>

      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Table </xsl:text>
          <xsl:value-of select="@uta:comment"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>5</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Type</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>0.5*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Length</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Data</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Comment</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Type</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Length</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Data</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Comment</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
              
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
          
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;      </xsl:text>

      <xsl:apply-templates mode="data"/>

    </xsl:element> <!--section -->
    <xsl:text>&#x000A;      </xsl:text>
  </xsl:template> <!-- uta:record -->

  <xsl:template match="uta:data" mode="data">
    <xsl:element name="table">
      <xsl:element name="title">
        <xsl:text>Default values </xsl:text>
        <xsl:value-of select="@uta:id"/>
      </xsl:element> <!-- title -->
      <xsl:text>&#x000A;      </xsl:text>
      <xsl:element name="tgroup">        
        <xsl:attribute name="cols"><xsl:text>3</xsl:text></xsl:attribute>
        <xsl:text>&#x000A;        </xsl:text>
        
        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>
        
        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>element</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>
        
        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>value</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>3*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>

        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>comment</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>3*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>

        <xsl:element name="thead">
          <xsl:text>&#x000A;          </xsl:text>
          <xsl:element name="row">
            <xsl:text>&#x000A;            </xsl:text>
            <xsl:element name="entry">
              <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
              <xsl:text>Id</xsl:text>
            </xsl:element> <!-- entry -->
            <xsl:text>&#x000A;            </xsl:text>
            <xsl:element name="entry">
              <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
              <xsl:attribute name="namest"><xsl:text>element</xsl:text></xsl:attribute>
              <xsl:attribute name="nameend"><xsl:text>comment</xsl:text></xsl:attribute>
              <xsl:text>data</xsl:text>
            </xsl:element> <!-- entry -->
            <xsl:text>&#x000A;            </xsl:text>
          </xsl:element> <!-- row -->
          <xsl:text>&#x000A;        </xsl:text>
        </xsl:element> <!-- thead -->
        <xsl:text>&#x000A;        </xsl:text>

        <xsl:element name="tbody">
          <xsl:text>&#x000A;          </xsl:text>
          <xsl:apply-templates mode="data"/>
        </xsl:element> <!-- tbody -->
        <xsl:text>&#x000A;        </xsl:text>
        
      </xsl:element> <!-- tgroup -->
      <xsl:text>&#x000A;      </xsl:text>
    </xsl:element> <!-- table -->
    <xsl:text>&#x000A;    </xsl:text>
  </xsl:template> <!-- uta:data-->

  <xsl:template match="uta:table">
    <xsl:element name="section">
      <xsl:attribute name="id"><xsl:value-of select="@uta:id"/></xsl:attribute>
      <xsl:attribute name="xreflabel"><xsl:value-of select="@uta:id"/></xsl:attribute>
      <xsl:element name="title">
        <xsl:text>Table </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>: </xsl:text>
        <xsl:value-of select="@uta:comment"/>
      </xsl:element>
      <xsl:text>&#x000A;      </xsl:text>

      <xsl:element name="para">
        <xsl:text>Settings: </xsl:text>
        <xsl:text>&#x000A;        </xsl:text>
        <xsl:element name="itemizedlist">
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:element name="listitem">
            <xsl:text>Row type: </xsl:text>
            <xsl:value-of select="@uta:type"/>
          </xsl:element> <!-- listitem -->
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:if test="@uta:length!=''">
            <xsl:element name="listitem">
              <xsl:text>Row length: </xsl:text>
              <xsl:call-template name="findval">
                <xsl:with-param name="len"><xsl:value-of select="@uta:length"/></xsl:with-param>
              </xsl:call-template>
            </xsl:element> <!-- listitem -->
            <xsl:text>&#x000A;          </xsl:text>
          </xsl:if>

          <xsl:element name="listitem">
            <xsl:text>Number of rows in the table: </xsl:text>
            <xsl:choose>
                <xsl:when test="@uta:dim!=''"><xsl:value-of select="@uta:dim"/></xsl:when>
                <xsl:otherwise><xsl:value-of select="count(uta:row)"/></xsl:otherwise>
            </xsl:choose>                    
          </xsl:element> <!-- listitem -->
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:element name="listitem">
            <xsl:text>Cache: </xsl:text>
              <xsl:choose>
                  <xsl:when test="@uta:cache='yes'"><xsl:text>Enabled</xsl:text></xsl:when>
                  <xsl:otherwise><xsl:text>Disabled</xsl:text></xsl:otherwise>
              </xsl:choose>                    
          </xsl:element> <!-- listitem -->
          <xsl:text>&#x000A;          </xsl:text>

        </xsl:element> <!-- itemizedlist-->
        <xsl:text>&#x000A;        </xsl:text>
      </xsl:element> <!-- para -->
      <xsl:text>&#x000A;      </xsl:text>

      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Table </xsl:text>
          <xsl:value-of select="@uta:comment"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>3</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Data</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Comment</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Data</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Comment</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
  
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;      </xsl:text>
    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;      </xsl:text>
  </xsl:template> <!-- uta:table -->

  <xsl:template match="uta:field">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="anchor">
          <xsl:with-param name="id"><xsl:value-of select="@uta:id"/></xsl:with-param>
        </xsl:call-template>
        <xsl:if test="@uta:export!=''">
          <xsl:text> (exported as [</xsl:text>
          <xsl:value-of select="@uta:export"/>
          <xsl:text>])</xsl:text>
        </xsl:if>
        <xsl:if test="@uta:repeat!=''">
          <xsl:text> (repeated </xsl:text>
          <xsl:value-of select="@uta:repeat"/>
          <xsl:text> times)</xsl:text>
        </xsl:if>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:value-of select="@uta:type"/>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findval">
          <xsl:with-param name="len"><xsl:value-of select="@uta:length"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:element name="computeroutput"><xsl:value-of select="@uta:data"/></xsl:element>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="@uta:comment"/></xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element>
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template> <!-- uta:field -->

  <xsl:template match="uta:row">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="anchor">
          <xsl:with-param name="id"><xsl:value-of select="@uta:id"/></xsl:with-param>
        </xsl:call-template>
        <xsl:if test="@uta:export!=''">
          <xsl:text> (exported as [</xsl:text>
          <xsl:value-of select="@uta:export"/>
          <xsl:text>])</xsl:text>
        </xsl:if>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:element name="computeroutput"><xsl:value-of select="@uta:data"/></xsl:element>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="@uta:comment"/></xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element>
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>

  <xsl:template match="uta:instance" mode="data">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="valign"><xsl:text>middle</xsl:text></xsl:attribute>
        <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
        <xsl:attribute name="morerows">
          <xsl:value-of select="count(uta:col)"/>
        </xsl:attribute>
        <xsl:choose>
            <xsl:when test="@uta:id!=''"><xsl:value-of select="@uta:id"/></xsl:when>
            <xsl:otherwise><xsl:value-of select="position()-1"/></xsl:otherwise>
        </xsl:choose>                    
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="namest"><xsl:text>element</xsl:text></xsl:attribute>
        <xsl:attribute name="nameend"><xsl:text>comment</xsl:text></xsl:attribute>
        <xsl:value-of select="@uta:comment"/>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:apply-templates mode="data"/>    
  </xsl:template> <!-- uta:instance -->

  <xsl:template match="uta:col" mode="data">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="position()-1"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:element name="computeroutput"><xsl:value-of select="@uta:data"/></xsl:element>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:element name="computeroutput"><xsl:value-of select="@uta:comment"/></xsl:element>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
  </xsl:template> <!-- uta:col-->

  <xsl:template match="uta:converters">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Converters</xsl:text>
      </xsl:element>
      <xsl:text>&#x000A;        </xsl:text>

      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Converters </xsl:text>
          <xsl:value-of select="@uta:id"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>3</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>element</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>value</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:attribute name="namest"><xsl:text>element</xsl:text></xsl:attribute>
                <xsl:attribute name="nameend"><xsl:text>value</xsl:text></xsl:attribute>
                <xsl:text>data</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
  
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;      </xsl:text>
    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;      </xsl:text>
  </xsl:template> <!-- converters -->
  
  <xsl:template match="uta:convert">
  <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="valign"><xsl:text>middle</xsl:text></xsl:attribute>
        <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
        <xsl:attribute name="morerows"><xsl:text>4</xsl:text></xsl:attribute>
        <xsl:call-template name="anchor">
          <xsl:with-param name="id"><xsl:value-of select="@uta:id"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="namest"><xsl:text>element</xsl:text></xsl:attribute>
        <xsl:attribute name="nameend"><xsl:text>value</xsl:text></xsl:attribute>
        <xsl:value-of select="@uta:comment"/>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>dbKey</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findid">
          <xsl:with-param name="id"><xsl:value-of select="uta:dbKey"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>dbFmt</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="uta:dbFmt"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>fmt</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="uta:fmt"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>len</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findval">
          <xsl:with-param name="len"><xsl:value-of select="uta:length"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
    
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template> <!-- convert -->

  <xsl:template match="uta:combos">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Combos</xsl:text>
      </xsl:element>
      <xsl:text>&#x000A;        </xsl:text>

      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Combos </xsl:text>
          <xsl:value-of select="@uta:id"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>3</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>element</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>value</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:attribute name="namest"><xsl:text>element</xsl:text></xsl:attribute>
                <xsl:attribute name="nameend"><xsl:text>value</xsl:text></xsl:attribute>
                <xsl:text>data</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
  
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;    </xsl:text>
    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;      </xsl:text>
  </xsl:template>

  <xsl:template match="uta:combo">  
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="valign"><xsl:text>middle</xsl:text></xsl:attribute>
        <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
        <xsl:attribute name="morerows"><xsl:text>5</xsl:text></xsl:attribute>
        <xsl:call-template name="anchor">
          <xsl:with-param name="id"><xsl:value-of select="@uta:id"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:attribute name="namest"><xsl:text>element</xsl:text></xsl:attribute>
        <xsl:attribute name="nameend"><xsl:text>value</xsl:text></xsl:attribute>
        <xsl:value-of select="@uta:comment"/>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>opr</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="uta:operation"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>arg0</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:if test="uta:arg0!=''">
          <xsl:call-template name="findid">
            <xsl:with-param name="id"><xsl:value-of select="uta:arg0"/></xsl:with-param>
          </xsl:call-template>
        </xsl:if>
        <xsl:if test="uta:arg0/@uta:value!=''">
          <xsl:text> Value: </xsl:text>
          <xsl:value-of select="uta:arg0/@uta:value"/>
        </xsl:if>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>arg1</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:if test="uta:arg1!=''">
          <xsl:call-template name="findid">
            <xsl:with-param name="id"><xsl:value-of select="uta:arg1"/></xsl:with-param>
          </xsl:call-template>
        </xsl:if>
        <xsl:if test="uta:arg1/@uta:value!=''">
          <xsl:text> Value: </xsl:text>
          <xsl:value-of select="uta:arg1/@uta:value"/>
        </xsl:if>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>arg2</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:if test="uta:arg2!=''">
          <xsl:call-template name="findid">
            <xsl:with-param name="id"><xsl:value-of select="uta:arg2"/></xsl:with-param>
          </xsl:call-template>
        </xsl:if>
        <xsl:if test="uta:arg2/@uta:value!=''">
          <xsl:text> Value: </xsl:text>
          <xsl:value-of select="uta:arg2/@uta:value"/>
        </xsl:if>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>

    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:text>len</xsl:text></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findval">
          <xsl:with-param name="len"><xsl:value-of select="uta:length"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:rptLayout">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Receipt Layout </xsl:text>
        <xsl:value-of select="@uta:id"/>
      </xsl:element>
      <xsl:text>&#x000A;        </xsl:text>

      <xsl:element name="para">
        <xsl:text>Settings: </xsl:text>
        <xsl:text>&#x000A;        </xsl:text>
        <xsl:element name="itemizedlist">
          <xsl:text>&#x000A;          </xsl:text>

          <xsl:if test="@uta:default!=''">
            <xsl:element name="listitem">
              <xsl:text>Default printing function: </xsl:text>
              <xsl:value-of select="@uta:default"/>
            </xsl:element> <!-- listitem -->
          <xsl:text>&#x000A;          </xsl:text>
          </xsl:if>


        </xsl:element> <!-- itemizedlist-->
        <xsl:text>&#x000A;        </xsl:text>
      </xsl:element> <!-- para -->
      <xsl:text>&#x000A;      </xsl:text>

      <xsl:apply-templates/>

    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;      </xsl:text>
  </xsl:template>
  
  <xsl:template match="uta:section">
    <xsl:element name="table">
      <xsl:attribute name="id"><xsl:value-of select="@uta:id"/></xsl:attribute>
      <xsl:attribute name="xreflabel"><xsl:value-of select="@uta:id"/></xsl:attribute>
      <xsl:element name="title">
        <xsl:text>Receipt section </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>: </xsl:text>
        <xsl:value-of select="@uta:comment"/>
      </xsl:element> <!-- title -->
      <xsl:text>&#x000A;      </xsl:text>
      <xsl:element name="tgroup">        
        <xsl:attribute name="cols"><xsl:text>4</xsl:text></xsl:attribute>
        <xsl:text>&#x000A;        </xsl:text>
        
        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>Esc</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>2*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>
        
        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>Type</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>

        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>Line</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>4*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>
        
        <xsl:element name="colspec">
          <xsl:attribute name="colname"><xsl:text>Comment</xsl:text></xsl:attribute>
          <xsl:attribute name="colwidth"><xsl:text>6*</xsl:text></xsl:attribute>
        </xsl:element> <!-- colspec -->
        <xsl:text>&#x000A;        </xsl:text>

        <xsl:element name="thead">
          <xsl:text>&#x000A;          </xsl:text>
          <xsl:element name="row">
            <xsl:text>&#x000A;            </xsl:text>
            <xsl:element name="entry">
              <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
              <xsl:text>Esc</xsl:text>
            </xsl:element> <!-- entry -->
            <xsl:text>&#x000A;            </xsl:text>
            <xsl:element name="entry">
              <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
              <xsl:text>Type</xsl:text>
            </xsl:element> <!-- entry -->
            <xsl:text>&#x000A;            </xsl:text>
            <xsl:element name="entry">
              <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
              <xsl:text>Data</xsl:text>
            </xsl:element> <!-- entry -->
            <xsl:text>&#x000A;            </xsl:text>
            <xsl:element name="entry">
              <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
              <xsl:text>Comment</xsl:text>
            </xsl:element> <!-- entry -->
            <xsl:text>&#x000A;          </xsl:text>
          </xsl:element> <!-- row -->
          <xsl:text>&#x000A;        </xsl:text>
        </xsl:element> <!-- thead -->
        <xsl:text>&#x000A;        </xsl:text>

        <xsl:element name="tbody">
          <xsl:text>&#x000A;          </xsl:text>
          <xsl:apply-templates/>
        </xsl:element> <!-- tbody -->
        <xsl:text>&#x000A;        </xsl:text>

      </xsl:element> <!-- tgroup -->
      <xsl:text>&#x000A;      </xsl:text>
    </xsl:element> <!-- table -->
    <xsl:text>&#x000A;      </xsl:text>
  </xsl:template>

  <xsl:template match="uta:line">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry"><xsl:value-of select="@uta:esc"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry">
        <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
        <xsl:text>L</xsl:text>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry">
        <xsl:call-template name="findid">
          <xsl:with-param name="id"><xsl:value-of select="."/></xsl:with-param>
        </xsl:call-template>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry">
        <xsl:element name="computeroutput"><xsl:value-of select="@uta:comment"/></xsl:element>
      </xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
      
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>

  <xsl:template match="uta:block">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry">
        <xsl:attribute name="namest"><xsl:text>Esc</xsl:text></xsl:attribute>
        <xsl:attribute name="nameend"><xsl:text>Type</xsl:text></xsl:attribute>
        <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
        <xsl:text>B</xsl:text>
      </xsl:element> <!-- entry -->      
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry">
        <xsl:call-template name="findid">
          <xsl:with-param name="id"><xsl:value-of select="."/></xsl:with-param>
        </xsl:call-template>
      </xsl:element> <!-- entry -->
      <xsl:text>&#x000A;            </xsl:text>
      
      <xsl:element name="entry"><xsl:value-of select="@uta:comment"/></xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
      
    </xsl:element> <!-- row -->
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>

  <xsl:template match="uta:find">
    <xsl:element name="section">
      <xsl:element name="title">
        <xsl:text>Find descriptors </xsl:text>
        <xsl:value-of select="@uta:id"/>
      </xsl:element> <!-- title -->
      <xsl:text>&#x000A;    </xsl:text>
      
      <xsl:element name="table">
        <xsl:element name="title">
          <xsl:text>Find</xsl:text>
          <xsl:value-of select="@uta:comment"/>
        </xsl:element> <!-- title -->
        <xsl:text>&#x000A;      </xsl:text>
        <xsl:element name="tgroup">        
          <xsl:attribute name="cols"><xsl:text>7</xsl:text></xsl:attribute>
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Id</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Where</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>How</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Dim</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Length</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>What</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>
          
          <xsl:element name="colspec">
            <xsl:attribute name="colname"><xsl:text>Comment</xsl:text></xsl:attribute>
            <xsl:attribute name="colwidth"><xsl:text>1*</xsl:text></xsl:attribute>
          </xsl:element> <!-- colspec -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="thead">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:element name="row">
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Id</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Where</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>How</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Dim</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Length</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>What</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;            </xsl:text>
              
              <xsl:element name="entry">
                <xsl:attribute name="align"><xsl:text>center</xsl:text></xsl:attribute>
                <xsl:text>Comment</xsl:text>
              </xsl:element> <!-- entry -->
              <xsl:text>&#x000A;          </xsl:text>
              
            </xsl:element> <!-- row -->
            <xsl:text>&#x000A;        </xsl:text>
          </xsl:element> <!-- thead -->
          <xsl:text>&#x000A;        </xsl:text>

          <xsl:element name="tbody">
            <xsl:text>&#x000A;          </xsl:text>
            <xsl:apply-templates/>
          </xsl:element> <!-- tbody -->
          <xsl:text>&#x000A;        </xsl:text>
  
        </xsl:element> <!-- tgroup -->
        <xsl:text>&#x000A;      </xsl:text>
      </xsl:element> <!-- table -->
      <xsl:text>&#x000A;    </xsl:text>
    </xsl:element> <!-- section -->
    <xsl:text>&#x000A;  </xsl:text>
  </xsl:template>

  <xsl:template match="uta:search|uta:index">
    <xsl:element name="row">
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="@uta:id"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:element name="xref">
          <xsl:attribute name="linkend"><xsl:value-of select="@uta:where"/></xsl:attribute>
        </xsl:element>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="@uta:how"/></xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findval">
          <xsl:with-param name="len"><xsl:value-of select="@uta:dim"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findval">
          <xsl:with-param name="len"><xsl:value-of select="@uta:length"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry">
        <xsl:call-template name="findid">
          <xsl:with-param name="len"><xsl:value-of select="@uta:what"/></xsl:with-param>
        </xsl:call-template>
      </xsl:element>
      <xsl:text>&#x000A;            </xsl:text>
      <xsl:element name="entry"><xsl:value-of select="@uta:comment"/></xsl:element>
      <xsl:text>&#x000A;          </xsl:text>
    </xsl:element>
    <xsl:text>&#x000A;          </xsl:text>
  </xsl:template>

</xsl:transform>
