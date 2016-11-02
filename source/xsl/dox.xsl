<?xml version="1.0"?>

<xsl:transform version = "1.0" id="dox"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>

<!-- extract the file list from ingedev 6 .project file convert it into doxygen configuration format -->

  <xsl:output method="text"/>
  <xsl:strip-space elements="*"/>

  <xsl:template match="name"/>
  <xsl:template match="nature"/>
  <xsl:template match="type"/>

  <xsl:template match="location">
    <xsl:text>                         </xsl:text>
    <xsl:value-of select="."/>
    <xsl:text> \ &#x000A;</xsl:text>
  </xsl:template>

</xsl:transform>
