cls
xsltproc ..\xsl\keyh.xsl dbs.xml > ..\key.h
xsltproc ..\xsl\maph.xsl dbs.xml > ..\map.h
xsltproc ..\xsl\dfth.xsl dbs.xml > ..\dft.h
xsltproc ..\xsl\mapc.xsl dbs.xml > ..\map.c
xsltproc ..\xsl\dftc.xsl dbs.xml > ..\dft.c
xsltproc ..\xsl\rptc.xsl dbs.xml > ..\rpt.c

pause