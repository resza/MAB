<?xml version="1.0"?>

<xsl:transform version = "1.0" id="mapc"
    extension-element-prefixes="uta"
    xmlns:xsl = "http://www.w3.org/1999/XSL/Transform"
    xmlns:uta = "http://www.ingenico.com/xml/uta"
>

<xsl:include href="inc.xsl"/>

<xsl:template match="/">
    <xsl:text>#include "global.h"&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build begKey() function -->
    <xsl:text>/** Find a key subspace corresponding to a given key&#010;</xsl:text>
    <xsl:text> * \param  key (I) Key to be located. &#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> *  - starting sentinel of a subspace that the key belongs to&#010;</xsl:text>
    <xsl:text> *  - negative if the input key is out of space&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * Used by other map functions to identify a data structure which the&#010;</xsl:text>
    <xsl:text> * data element belongs to&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>static int begKey(word key){ //find starting sentinel of key subspace&#010;</xsl:text>
    <xsl:apply-templates mode="begKey"/>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
            
    <!-- Build relKey() function -->
    <xsl:text>/** Find relative position of a key within its subspace&#010;</xsl:text>
    <xsl:text> * \param  key (I) Key to be adjusted. &#010;</xsl:text>
    <xsl:text> * \return - relative position of a key within its subspace&#010;</xsl:text>
    <xsl:text> * \pre key should be a valid key of a data element (inside key enum)&#010;</xsl:text>
    <xsl:text> * Used by other map functions to find field numbder within a record,&#010;</xsl:text>
    <xsl:text> * or row number within a table, etc&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>word relKey(word key){ //return relative key position in key subspace&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    return key-beg-1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build getContainer() function -->
    <xsl:text>/** Find a container corresponding to a given key&#010;</xsl:text>
    <xsl:text> * \param  key (I) Key whose contatiner should be identified. &#010;</xsl:text>
    <xsl:text> * \return pointer to the container that contains the key&#010;</xsl:text>
    <xsl:text> * \pre key should be a valid key of a data element (inside key enum)&#010;</xsl:text>
    <xsl:text> * A given key can belong to a record, a table, a converter or a combo.&#010;</xsl:text>
    <xsl:text> * For each of these data structures a surrounding container is constructed.&#010;</xsl:text>
    <xsl:text> * The derivated elements (combos and converters) use these containers&#010;</xsl:text>
    <xsl:text> * to perform operations.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>const tContainer *getContainer(word key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="getContainer"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return 0;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <xsl:apply-templates/>
    
    <!-- Build mapInit() function -->
    <xsl:text>/** Call the constructors for all UTA data base structures:&#010;</xsl:text>
    <xsl:text> * - tables&#010;</xsl:text>
    <xsl:text> * - records&#010;</xsl:text>
    <xsl:text> * - multirecords&#010;</xsl:text>
    <xsl:text> * - converters&#010;</xsl:text>
    <xsl:text> * - combos&#010;</xsl:text>
    <xsl:text> * - containers&#010;</xsl:text>
    <xsl:text> * - volatile record&#010;</xsl:text>
    <xsl:text> * - search descriptors&#010;</xsl:text>
    <xsl:text> * - indices and keys&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - full size of DFS in bytes occupied by UTA data base&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The data elements are saved in DFS in the same order as they are described&#010;</xsl:text>
    <xsl:text> * in dbs.xml file.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * In this function the descriptors are filled.&#010;</xsl:text>
    <xsl:text> * All the operations are performed in RAM; it is fast.&#010;</xsl:text>
    <xsl:text> * Usually this function is called during POWER ON event treatment.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The size of each data structure is traced.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapInit(void){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>    byte sec;&#010;</xsl:text>
    <xsl:text>    card ofs;&#010;</xsl:text>
    <xsl:text>    word len;&#010;</xsl:text>
    <xsl:text>    card tot= 0;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    trcS("mapInit: Beg\n");&#010;</xsl:text>
    <xsl:apply-templates mode="mapInit"/>
    <xsl:text>    trcFN("total DFS size: %d\n",tot);&#010;</xsl:text>
    <xsl:text>    ret= tot;&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    trcFN("mapInit: ret=%d\n",ret);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapReset() function -->
    <xsl:text>/** Fill a data structure by zeroes.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of the data structure to be reset.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the size of data structure to be reset&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If the parameter is keyBeg (zero) all data structures are reset.&#010;</xsl:text>
    <xsl:text> * If the data structure is in DFS it can take some time.&#010;</xsl:text>
    <xsl:text> * Usually volatile record is reset before any event treatment.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapReset(word key){&#010;</xsl:text>
    <xsl:text>    int ret= 0;&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>    trcFN("mapReset: key=%d\n",key);&#010;</xsl:text>
    <xsl:text>    if(key==keyBeg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapResetAll"/>
    <xsl:text>        goto lblEnd;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    if(beg &lt; 0) beg= begKey((word)(key+1));&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapReset1"/>
    <xsl:text>        default: ret= -1;&#010;</xsl:text>
    <xsl:text>            break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    CHECK(ret>0,lblKO);&#010;</xsl:text>
    <xsl:text>    goto lblEnd;&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    trcErr(ret);&#010;</xsl:text>
    <xsl:text>    ret= -1;&#010;</xsl:text>
    <xsl:text>lblEnd:&#010;</xsl:text>
    <xsl:text>    trcFN("mapReset: ret=%d\n",ret);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapTabGet() function -->
    <xsl:text>/** Retrieve a data element from a table.&#010;</xsl:text>
    <xsl:text> * \param  tab (I) Pointer to table descriptor.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a row within the table.&#010;</xsl:text>
    <xsl:text> * \param  ptr (O) Pointer to buffer where to write the data to.&#010;</xsl:text>
    <xsl:text> * \param  len (I) Buffer length.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - size of the data element retrieved&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The parameter len ensures that the memory is not overwritten.&#010;</xsl:text>
    <xsl:text> * If it is too small the exception condition is arised.&#010;</xsl:text>
    <xsl:text> * Zero length is treated as table width.&#010;</xsl:text>
    <xsl:text> * This way of calling the function is not recommended.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre tab!=0&#010;</xsl:text>
    <xsl:text> * \pre ptr!=0&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapGet()&#010;</xsl:text>
    <xsl:text> *  - mapRecGet()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>static int mapTabGet(const tTable *tab,word key,void *ptr,word len){&#010;</xsl:text>
    <xsl:text>    if(!len) len= tabWdt(tab);&#010;</xsl:text>
    <xsl:text>    if(len&lt;tabWdt(tab)) return -1;&#010;</xsl:text>
    <xsl:text>    return tabGet(tab,key,ptr,len);&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapRecGet() function -->
    <xsl:text>/** Retrieve a data element from a record.&#010;</xsl:text>
    <xsl:text> * \param  rec (I) Pointer to record descriptor.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a field within the record.&#010;</xsl:text>
    <xsl:text> * \param  ptr (O) Pointer to buffer where to write the data to.&#010;</xsl:text>
    <xsl:text> * \param  len (I) Buffer length.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - size of the data element retrieved&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The parameter len ensures that the memory is not overwritten.&#010;</xsl:text>
    <xsl:text> * If it is too small the exception condition is arised.&#010;</xsl:text>
    <xsl:text> * Zero length is treated as field length.&#010;</xsl:text>
    <xsl:text> * This way of calling the function is not recommended.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre rec!=0&#010;</xsl:text>
    <xsl:text> * \pre ptr!=0&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapGet()&#010;</xsl:text>
    <xsl:text> *  - mapTabGet()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>static int mapRecGet(const tRecord *rec,word key,void *ptr,word len){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>    if(!len) len= rec->len[key];&#010;</xsl:text>
    <xsl:text>    if(len&lt;rec->len[key]) return -1;&#010;</xsl:text>
    <xsl:text>    ret= recGet(rec,key,ptr,len);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapGet() function -->    
    <xsl:text>/** Retrieve a data element from a data structure.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a data element.&#010;</xsl:text>
    <xsl:text> * \param  ptr (O) Pointer to buffer where to write the data to.&#010;</xsl:text>
    <xsl:text> * \param  len (I) Buffer length.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - size of the data element retrieved&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The parameter len ensures that the memory is not overwritten.&#010;</xsl:text>
    <xsl:text> * If it is too small the exception condition is arised.&#010;</xsl:text>
    <xsl:text> * Zero length is treated as field length.&#010;</xsl:text>
    <xsl:text> * This way of calling the function is not recommended.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre ptr!=0&#010;</xsl:text>
    <xsl:text> * \pre key belongs to the key space&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * At first the key subspace is identified that contains the data element.&#010;</xsl:text>
    <xsl:text> * Then the relative position of the key within the subspace is calculated.&#010;</xsl:text>
    <xsl:text> * Depending on the subspace the retrieval function is called for related descriptor.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapTabGet()&#010;</xsl:text>
    <xsl:text> *  - mapRecGet()&#010;</xsl:text>
    <xsl:text> *  - mapPut()&#010;</xsl:text>
    <xsl:text> *  - mapMove()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapGet(word key,void *ptr,word len){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    key-= beg+1;&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapGet"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapTabPut() function -->    
    <xsl:text>/** Save a data element into a table of rows.&#010;</xsl:text>
    <xsl:text> * \param  tab (I) Pointer to table descriptor.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a row within the table.&#010;</xsl:text>
    <xsl:text> * \param  ptr (I) Pointer to buffer containing the data to be saved.&#010;</xsl:text>
    <xsl:text> * \param  len (I) Buffer length.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - size of the data element saved&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre tab!=0&#010;</xsl:text>
    <xsl:text> * \pre ptr!=0&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If len is to big the buffer is truncated to the size of data element.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapPut()&#010;</xsl:text>
    <xsl:text> *  - mapRecPut()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>static int mapTabPut(tTable *tab,word key,const void *ptr,word len){&#010;</xsl:text>
    <xsl:text>    if(len&gt;tabWdt(tab)) len= tabWdt(tab);&#010;</xsl:text>
    <xsl:text>    return tabPut(tab,key,ptr,len);&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapRecPut() function -->    
    <xsl:text>/** Save a data element into a record.&#010;</xsl:text>
    <xsl:text> * \param  rec (I) Pointer to record descriptor.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a field within the record.&#010;</xsl:text>
    <xsl:text> * \param  ptr (I) Pointer to buffer containing the data to be saved.&#010;</xsl:text>
    <xsl:text> * \param  len (I) Buffer length.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - size of the data element saved&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre rec!=0&#010;</xsl:text>
    <xsl:text> * \pre ptr!=0&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If len is to big the buffer is truncated to the size of data element.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapPut()&#010;</xsl:text>
    <xsl:text> *  - mapTabPut()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>static int mapRecPut(tRecord *rec,word key,const void *ptr,word len){&#010;</xsl:text>
    <xsl:text>    int ret;&#010;</xsl:text>
    <xsl:text>    if(len&gt;rec->len[key]) len= rec->len[key];&#010;</xsl:text>
    <xsl:text>    ret= recPut(rec,key,ptr,len);&#010;</xsl:text>
    <xsl:text>    return ret;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapPut() function -->    
    <xsl:text>/** Save a data element into a data structure.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a data element.&#010;</xsl:text>
    <xsl:text> * \param  ptr (I) Pointer to buffer containing the data to be saved.&#010;</xsl:text>
    <xsl:text> * \param  len (I) Buffer length.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - size of the data element saved&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre ptr!=0&#010;</xsl:text>
    <xsl:text> * \pre key belongs to the key space&#010;</xsl:text>
    <xsl:text> * At first the key subspace is identified that contains the data element.&#010;</xsl:text>
    <xsl:text> * Then the relative position of the key within the subspace is calculated.&#010;</xsl:text>
    <xsl:text> * Depending on the subspace the save function is called for related descriptor.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If len is to big the buffer is truncated to the size of data element.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapTabPut()&#010;</xsl:text>
    <xsl:text> *  - mapRecPut()&#010;</xsl:text>
    <xsl:text> *  - mapGet()&#010;</xsl:text>
    <xsl:text> *  - mapMove()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapPut(word key,const void *ptr,word len){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    key-= beg+1;&#010;</xsl:text>
    <xsl:text>    if(!len) len= strlen((char *)ptr);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapPut"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapDatLen() function -->    
    <xsl:text>/** Get data element length.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a data element.&#010;</xsl:text>
    <xsl:text> * \return data element length&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre key belongs to the key space&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * At first the key subspace is identified that contains the data element.&#010;</xsl:text>
    <xsl:text> * Then the relative position of the key within the subspace is calculated.&#010;</xsl:text>
    <xsl:text> * Depending on the subspace a function is called for related descriptor.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>word mapDatLen(word key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>    const tContainer *cnt;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    cnt= getContainer(key);&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    key-= beg+1;&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapDatLen"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return 0;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapDatFmt() function -->    
    <xsl:text>/** Get data element format:&#010;</xsl:text>
    <xsl:text> * - 'A' for ASCII strings&#010;</xsl:text>
    <xsl:text> * - 'B' for binary data&#010;</xsl:text>
    <xsl:text> * - 'N' for numeric values&#010;</xsl:text>
    <xsl:text> * - 'X' for combos&#010;</xsl:text>
    <xsl:text> * - 'C' for converters&#010;</xsl:text>
    <xsl:text> * - 0; error&#010;</xsl:text>
    <xsl:text> * \param  key (I) Index of a data element.&#010;</xsl:text>
    <xsl:text> * \return data element length&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre key belongs to the key space&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * At first the key subspace is identified that contains the data element.&#010;</xsl:text>
    <xsl:text> * Then the relative position of the key within the subspace is calculated.&#010;</xsl:text>
    <xsl:text> * Depending on the subspace a function is called for related descriptor.&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>char mapDatFmt(word key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    key-= beg+1;&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapDatFmt"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return 0;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapMove() function -->    
    <xsl:text>/** Move the cursor within a multirecord.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of the multirecord.&#010;</xsl:text>
    <xsl:text> * \param  idx (I) Location where to move cursor.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - cursor position&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre key belongs to the key space&#010;</xsl:text>
    <xsl:text> * \pre idx is less than multirecord dimension&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * At first the multirecord key subspace is identified.&#010;</xsl:text>
    <xsl:text> * For this key subspace there are.&#010;</xsl:text>
    <xsl:text> *  - table descriptor, each row containing an instance of the multirecord&#010;</xsl:text>
    <xsl:text> *  - record descriptor that overlays a row in the table&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The record descriptor is moved to point to the record with index idx&#010;</xsl:text>
    <xsl:text> * within the multirecord.&#010;</xsl:text>
    <xsl:text> * The first position has index 0.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * It is a very fast operation, no DFS access is used.&#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapGet()&#010;</xsl:text>
    <xsl:text> *  - mapPut()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapMove(word key,word idx){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>    tTable *tab;&#010;</xsl:text>
    <xsl:text>    tRecord *rec;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    if(beg &lt; 0) beg= begKey((word)(key+1));&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    tab= 0;&#010;</xsl:text>
    <xsl:text>    rec= 0;&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapMove"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return recMove(rec,tab,idx);&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapCache() function -->    
    <xsl:text>/** Enable/disable cache buffer for a data structure.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of a data structure.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the size of allocated cache buffer&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre cache buffer should be already allocated for the data structure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * A cache buffer can be allocated for&#010;</xsl:text>
    <xsl:text> *  - a table: the size of cache buffer is equal to table size&#010;</xsl:text>
    <xsl:text> *  - a single record: the size of cache buffer is equal to record size&#010;</xsl:text>
    <xsl:text> *  - a multirecord: the size of cache buffer is equal to single record size&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * Cache buffer can be allocated for data residing in non-volatile memory to accelerate access to these data.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * The cache buffer is allocated for a data structure if its attribute&#010;</xsl:text>
    <xsl:text> * uta:cache is set to "yes".&#010;</xsl:text>
    <xsl:text> * By default it is disabled.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If cache buffer is enabled all mapGet()/mapPut() calls operate with&#010;</xsl:text>
    <xsl:text> * the data in cache buffer at a related offset.&#010;</xsl:text>
    <xsl:text> * Otherwise the real read/write operations are performed.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * To enable a cache buffer the function mapCache should be called&#010;</xsl:text>
    <xsl:text> * with a parameter key equal to the starting sentinel of the data structure, for example, mnuBeg.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * To disable cache buffer, the negative value should be provided, for example, (-mnuBeg).&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * Cache buffers can be used in the following context:&#010;</xsl:text>
    <xsl:text> *  - for essentially read/only data structures like menu or message table&#010;</xsl:text>
    <xsl:text> *    it can be loaded into cache buffer during POWER ON event treatment using mapLoad function.&#010;</xsl:text>
    <xsl:text> *    After that mapPut operations will operate with this RAM copy of&#010;</xsl:text>
    <xsl:text> *    the data structure.&#010;</xsl:text>
    <xsl:text> *    Note that for dynamical menu management a call to mapSave() or mapFlush()&#010;</xsl:text>
    <xsl:text> *    can be needed if the menu is modified.&#010;</xsl:text>
    <xsl:text> *  - For multirecord saving like transaction log save all the fields&#010;</xsl:text>
    <xsl:text> *    in the current record can be saved at first into cache buffer.&#010;</xsl:text>
    <xsl:text> *    When the record is filled in RAM it the mapSave() function is called to perform&#010;</xsl:text>
    <xsl:text> *    write operation.&#010;</xsl:text>
    <xsl:text> *    The low performance write operation is called only once in this case instead of&#010;</xsl:text>
    <xsl:text> *    performing it for each field of the record.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapLoad()&#010;</xsl:text>
    <xsl:text> *  - mapSave()&#010;</xsl:text>
    <xsl:text> *  - mapFlush()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapCache(int key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>    word tmp;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    tmp= (word)((key>0) ? key : (-key));&#010;</xsl:text>
    <xsl:text>    beg= begKey(tmp);&#010;</xsl:text>
    <xsl:text>    if(beg &lt; 0) beg= begKey((word)(tmp+1));&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapCache"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
    <!-- Build mapLoad() function -->    
    <xsl:text>/** Depending on the argument the function does:&#010;</xsl:text>
    <xsl:text> *  - For data structures where cache is enabled:&#010;</xsl:text>
    <xsl:text> *    load data from non-volatile memory to a cache buffer.&#010;</xsl:text>
    <xsl:text> *  - For indices: load index file into volatile memory.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of a data structure.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> *  - the size of buffer&#010;</xsl:text>
    <xsl:text> *  - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre cache buffer should be enabled for the data structure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapFlush()&#010;</xsl:text>
    <xsl:text> *  - mapSave()&#010;</xsl:text>
    <xsl:text> *  - mapCache()&#010;</xsl:text>
    <xsl:text> *  - mapFind()&#010;</xsl:text>
    <xsl:text> *  - mapIns()&#010;</xsl:text>
    <xsl:text> *  - mapDel()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapLoad(word key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    if(beg &lt; 0) beg= begKey((word)(key+1));&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapLoad"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapSave() function -->    
    <xsl:text>/** Depending on the argument the function does:&#010;</xsl:text>
    <xsl:text> *  - For data structures where cache is enabled:&#010;</xsl:text>
    <xsl:text> *    save all the data from a cache buffer to non-volatile memory.&#010;</xsl:text>
    <xsl:text> *  - For indices: save index from volatile memory into a file.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of a data structure.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> *  - the size of buffer&#010;</xsl:text>
    <xsl:text> *  - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre cache buffer should be enabled for the data structure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapFlush()&#010;</xsl:text>
    <xsl:text> *  - mapLoad()&#010;</xsl:text>
    <xsl:text> *  - mapCache()&#010;</xsl:text>
    <xsl:text> *  - mapFind()&#010;</xsl:text>
    <xsl:text> *  - mapIns()&#010;</xsl:text>
    <xsl:text> *  - mapDel()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapSave(word key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    if(beg &lt; 0) beg= begKey((word)(key+1));&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapSave"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapFlush() function -->    
    <xsl:text>/** Save the modified data from a cache buffer to non-volatile memory.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of a data structure.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the size of allocated cache buffer&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre cache buffer should be enabled for the data structure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If the data structure was loaded into cache memory before calling mapFlush()&#010;</xsl:text>
    <xsl:text> * the operation has the same effect as mapSave() but it can be faster for data structures of big size.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * If a single instance of multirecord is filled by some data mapSave() function&#010;</xsl:text>
    <xsl:text> * should be called to fill all the bytes in the record.&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapLoad()&#010;</xsl:text>
    <xsl:text> *  - mapSave()&#010;</xsl:text>
    <xsl:text> *  - mapCache()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapFlush(word key){&#010;</xsl:text>
    <xsl:text>    int beg;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    beg= begKey(key);&#010;</xsl:text>
    <xsl:text>    if(beg &lt; 0) beg= begKey((word)(key+1));&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    switch(beg){&#010;</xsl:text>
    <xsl:apply-templates mode="mapFlush"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapFind() function -->    
    <xsl:text>/** Find a pattern pat according to search descriptor key.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of the search/index descriptor&#010;</xsl:text>
    <xsl:text> * \param  pat (I) pattern to look for&#010;</xsl:text>
    <xsl:text> * \param  beg (I) starting position for search&#010;</xsl:text>
    <xsl:text> * \param  end (I) ending position for search&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the position of the record found&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * Four types of search operations are implemented in UTA:&#010;</xsl:text>
    <xsl:text> * - linear: (uta:find/\@uta:how="linear") sequential search in a multirecord.&#010;</xsl:text>
    <xsl:text> *   All records are browsed starting from beg until to end.&#010;</xsl:text>
    <xsl:text> *   For each single record the pattern is compared with a data element&#010;</xsl:text>
    <xsl:text> *   described by uta:what attribute in uta:find tag.&#010;</xsl:text>
    <xsl:text> *   This method can be applied for any multirecord. It is the slowest one.&#010;</xsl:text>
    <xsl:text> * - binary: (uta:find/\@uta:how="binary") binary search in a multirecord.&#010;</xsl:text>
    <xsl:text> *   This method can be applied for multirecords sorted according to the uta:what key.&#010;</xsl:text>
    <xsl:text> *   It is faster than linear but it is limited to sorted multirecords only.&#010;</xsl:text>
    <xsl:text> * - index: (uta:len missing in uta:index descriptor) binary search in index array.&#010;</xsl:text>
    <xsl:text> *   An index is an array of row numbers sorted according to a key uta:what.&#010;</xsl:text>
    <xsl:text> *   It has the same speed as binary search but it requires resources:&#010;</xsl:text>
    <xsl:text> *   a buffer should be allocated in volatile and non-volatile memory&#010;</xsl:text>
    <xsl:text> *   This method can be applied for multirecords sorted according to the uta:what key.&#010;</xsl:text>
    <xsl:text> * - key: (non-zero uta:len in uta:index descriptor) binary search in key array.&#010;</xsl:text>
    <xsl:text> *   A key array contains not only the row numbers but also the key values.&#010;</xsl:text>
    <xsl:text> *   The search is performed in RAM buffer, only one record at the end is required&#010;</xsl:text>
    <xsl:text> *   It is the fastest method, but it requires more resources than index array.&#010;</xsl:text>
    <xsl:text> *   &#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the key should reference an index&#010;</xsl:text>
    <xsl:text> * \pre pat!=0&#010;</xsl:text>
    <xsl:text> * \pre beg and end should be inside the range&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapIns()&#010;</xsl:text>
    <xsl:text> *  - mapDel()&#010;</xsl:text>
    <xsl:text> *  - mapLoad()&#010;</xsl:text>
    <xsl:text> *  - mapSave()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapFind(word key,const char *pat,word beg,word end){&#010;</xsl:text>
    <xsl:text>    switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapFind"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapIns() function -->    
    <xsl:text>/** Insert the current position of a multirecord into the index.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of the index.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the position of the current record&#010;</xsl:text>
    <xsl:text> * - (-1) if failure&#010;</xsl:text>
    <xsl:text> * - (-2) if key is duplicated&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the key should reference an index&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapFind()&#010;</xsl:text>
    <xsl:text> *  - mapDel()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapIns(word key){&#010;</xsl:text>
    <xsl:text>    word rel;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    rel= relKey(key);&#010;</xsl:text>
    <xsl:text>    switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapIns"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapDel() function -->    
    <xsl:text>/** Remove the current position of a multirecord from the index.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Starting sentinel of the index.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the position of the current record&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the key should reference an index&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - mapFind()&#010;</xsl:text>
    <xsl:text> *  - mapIns()&#010;</xsl:text>
    <xsl:text> */&#010;</xsl:text>
    <xsl:text>int mapDel(word key){&#010;</xsl:text>
    <xsl:text>    word rel;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    rel= relKey(key);&#010;</xsl:text>
    <xsl:text>    switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapDel"/>
    <xsl:text>        default: break;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

    <!-- Build mapExport() function -->    
    <xsl:text>/** Export a data element to a non-volatile data section.&#010;</xsl:text>
    <xsl:text> * \param  key (I) Valid non-deriveted data element key.&#010;</xsl:text>
    <xsl:text> * \param  sec (I) Data section where to export.&#010;</xsl:text>
    <xsl:text> * \return&#010;</xsl:text>
    <xsl:text> * - the length of string exported&#010;</xsl:text>
    <xsl:text> * - negative if failure&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \pre the key should reference a row of a table or a field of a record&#010;</xsl:text>
    <xsl:text> * \pre the key be described in dbs.xml with the attribute uta:export&#010;</xsl:text>
    <xsl:text> * &#010;</xsl:text>
    <xsl:text> * \sa&#010;</xsl:text>
    <xsl:text> *  - dbaImport()&#010;</xsl:text>
    <xsl:text> *  - dbaExport()&#010;</xsl:text>
    <xsl:text> *  - tabExport()&#010;</xsl:text>
    <xsl:text> *  - recExport()&#010;</xsl:text>
    <xsl:text> */ &#010;</xsl:text>
    <xsl:text>int mapExport(int key,byte sec){&#010;</xsl:text>
    <xsl:text>    int ret= 0;&#010;</xsl:text>
    <xsl:text>    word rel;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    if (key &#60; 0){&#010;</xsl:text>
    <xsl:text>        tTable *tab;&#010;</xsl:text>
    <xsl:text>        word idx;&#010;</xsl:text>
    <xsl:text>        word dim;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>        key= begKey((word)(-key));&#010;</xsl:text>
    <xsl:text>        tab= 0;&#010;</xsl:text>
    <xsl:text>        switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapGetRecDim"/>
    <xsl:text>            default: return -1;&#010;</xsl:text>
    <xsl:text>        };&#010;</xsl:text>
    <xsl:text>        dim= tabDim(tab);&#010;</xsl:text>
    <xsl:text>        for (idx= 0; idx &#60; dim; idx++){&#010;</xsl:text>
    <xsl:text>            mapMove((word)key,idx);&#010;</xsl:text>
    <xsl:apply-templates mode="mapExportDim"/>
    <xsl:text>        }&#010;</xsl:text>
    <xsl:text>        return dim;&#010;</xsl:text>
    <xsl:text>    }else{&#010;</xsl:text>
    <xsl:text>        if(key!=begKey((word)key)){&#010;</xsl:text>
    <xsl:text>            rel= relKey((word)key);&#010;</xsl:text>
    <xsl:text>            switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapExport"/>
    <xsl:text>                default: break;&#010;</xsl:text>
    <xsl:text>            }&#010;</xsl:text>
    <xsl:text>        }else{&#010;</xsl:text>
    <xsl:text>            word idx,beg,end,num;&#010;</xsl:text>
    <xsl:text>            char exp;&#010;</xsl:text>
    <xsl:text>            char *str;&#010;</xsl:text>
    <xsl:text>            tTable *tab;&#010;</xsl:text>
    <xsl:text>            char tmp[256];&#010;</xsl:text>
    <xsl:text>            char buf[256];&#010;</xsl:text>
     <xsl:text>&#010;</xsl:text>
    <xsl:text>            beg= 0;&#010;</xsl:text>
    <xsl:text>            end= 0;&#010;</xsl:text>
    <xsl:text>            exp= 0;&#010;</xsl:text>
    <xsl:text>            tab= 0;&#010;</xsl:text>
    <xsl:text>            str= 0;&#010;</xsl:text>
    <xsl:text>            switch(begKey((word)key)){&#010;</xsl:text>
    <xsl:apply-templates mode="mapExportGroup"/>
    <xsl:text>                default:&#010;</xsl:text>
    <xsl:text>                    break;&#010;</xsl:text>
    <xsl:text>            }&#010;</xsl:text>
    <xsl:text>            for(idx= beg+1; idx&lt;end; idx++){&#010;</xsl:text>
    <xsl:text>                num= idx-beg-1;;&#010;</xsl:text>
    <xsl:text>                strcpy(buf,str);&#010;</xsl:text>
    <xsl:text>                strcat(buf,"[");&#010;</xsl:text>
    <xsl:text>                num2dec(tmp,num,0);&#010;</xsl:text>
    <xsl:text>                strcat(buf,tmp);&#010;</xsl:text>
    <xsl:text>                strcat(buf,"]");&#010;</xsl:text>
    <xsl:text>                ret= tabExport(tab,num,sec,'S',buf);&#010;</xsl:text>
    <xsl:text>                if(ret&lt;0) return -1;&#010;</xsl:text>
    <xsl:text>            }&#010;</xsl:text>
    <xsl:text>            return end-beg-1;&#010;</xsl:text>
    <xsl:text>        }&#010;</xsl:text>
    <xsl:text>        goto lblKO;&#010;</xsl:text>
    <xsl:text>    }&#010;</xsl:text>
    <xsl:text>lblKO:&#010;</xsl:text>
    <xsl:text>    return -1;&#010;</xsl:text>
    <xsl:text>}&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    
</xsl:template>
    
<xsl:template match="uta:dbk"/>
<xsl:template match="uta:keySpace"><xsl:apply-templates/></xsl:template>
<xsl:template match="uta:dfsFile"/>
<xsl:template match="uta:volatile"/>
<xsl:template match="uta:derivated">
        <xsl:apply-templates/>
</xsl:template>
<xsl:template match="uta:rptLayout"/>

<xsl:template match="uta:converters">
        <!-- Build initCvt() function -->
        <xsl:text>//fill all converters in cvt array using cvtTable configuration table&#010;</xsl:text>
        <xsl:text>static int initCvt(void){&#010;</xsl:text>
        <xsl:text>    int ret;&#010;</xsl:text>
        <xsl:text>    word cvtKey;&#010;</xsl:text>
        <xsl:text>    word idx;&#010;</xsl:text>
        <xsl:text>    word dbKey;&#010;</xsl:text>
        <xsl:text>    const tContainer *cnt;&#010;</xsl:text>
        <xsl:text>    byte dbf;&#010;</xsl:text>
        <xsl:text>    byte fmt;&#010;</xsl:text>
        <xsl:text>    word len;&#010;</xsl:text>
        <xsl:text>    trcS("initCvt\n");&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>    for(cvtKey= cvtBeg+1; cvtKey&lt;cvtEnd; cvtKey++){&#010;</xsl:text>
        <xsl:text>        idx= cvtKey-cvtBeg-1;&#010;</xsl:text>
        <xsl:text>        dbKey= relKey(cvtTable[idx].key);&#010;</xsl:text>
        <xsl:text>        cnt= getContainer(cvtTable[idx].key); &#010;</xsl:text>
        <xsl:text>        dbf= cvtTable[idx].dbf;&#010;</xsl:text>
        <xsl:text>        fmt= cvtTable[idx].fmt;&#010;</xsl:text>
        <xsl:text>        len= cvtTable[idx].len;&#010;</xsl:text>
        <xsl:text>        ret= cvtInit(&amp;cvt[idx],cnt,dbKey,dbf,fmt,len);&#010;</xsl:text>
        <xsl:text>        CHECK(ret>0,lblKO);&#010;</xsl:text>
        <xsl:text>        ret= cntInit(&amp;cntCvt[idx],'c',&amp;cvt[idx]);&#010;</xsl:text>
        <xsl:text>        CHECK(ret>0,lblKO);&#010;</xsl:text>
        <xsl:text>    }&#010;</xsl:text>
        <xsl:text>    ret= 1;&#010;</xsl:text>
        <xsl:text>    goto lblEnd;&#010;</xsl:text>
        <xsl:text>lblKO:&#010;</xsl:text>
        <xsl:text>    trcErr(ret);&#010;</xsl:text>
        <xsl:text>    ret= -1;&#010;</xsl:text>
        <xsl:text>lblEnd:&#010;</xsl:text>
        <xsl:text>    return ret;&#010;</xsl:text>
        <xsl:text>}&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>

        <!-- Build mapCvtGet() function -->
        <xsl:text>static int mapCvtGet(word key,void *ptr,word len){&#010;</xsl:text>
        <xsl:text>    int ret;&#010;</xsl:text>
        <xsl:text>    ret= cvtGet(&amp;cvt[key],NULLKEY,ptr,len);&#010;</xsl:text>
        <xsl:text>    return ret;&#010;</xsl:text>
        <xsl:text>}&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>

        <!-- Build mapCvtPut() function -->    
        <xsl:text>static int mapCvtPut(word key,const void *ptr,word len){&#010;</xsl:text>
        <xsl:text>    int ret;&#010;</xsl:text>
        <xsl:text>    ret= cvtPut(&amp;cvt[key],0,ptr,len);&#010;</xsl:text>
        <xsl:text>    return ret;&#010;</xsl:text>
        <xsl:text>}&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos">
        <!-- Build getLocator() function -->
        <xsl:text>//if arg negative, treat -arg as argument value (cnt=0)&#010;</xsl:text>
        <xsl:text>//otherwise using a key value arg find the related container and relative key&#010;</xsl:text>
        <xsl:text>static void getLocator(tLocator *loc,int arg){&#010;</xsl:text>
        <xsl:text>    if(arg&lt;0){ //no container, argument is transferred by value&#010;</xsl:text>
        <xsl:text>        loc-&gt;cnt= 0;&#010;</xsl:text>
        <xsl:text>        loc-&gt;key= (word)(-arg);&#010;</xsl:text>
        <xsl:text>    }else{&#010;</xsl:text>
        <xsl:text>        loc-&gt;cnt= getContainer((word)arg);&#010;</xsl:text>
        <xsl:text>        switch(loc->cnt-&gt;typ){&#010;</xsl:text>
        <xsl:text>            case 't': case 'r': loc-&gt;key= relKey((word)arg); break; //table,record&#010;</xsl:text>
        <xsl:text>            case 'c': case 'x': loc-&gt;key= NULLKEY; break; //converters and combos arrays&#010;</xsl:text>
        <xsl:text>            default: break; //unexpected type&#010;</xsl:text>
        <xsl:text>        }&#010;</xsl:text>
        <xsl:text>    }&#010;</xsl:text>
        <xsl:text>}&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>

        <!-- Build initCmb() function -->
        <xsl:text>//fill all combos in cmb array using cmbTable configuration table&#010;</xsl:text>
        <xsl:text>static int initCmb(void){&#010;</xsl:text>
        <xsl:text>    int ret;&#010;</xsl:text>
        <xsl:text>    tLocator loc[3];&#010;</xsl:text>
        <xsl:text>    word cmbKey;&#010;</xsl:text>
        <xsl:text>    word idx;&#010;</xsl:text>
        <xsl:text>    byte opr;&#010;</xsl:text>
        <xsl:text>    word len;&#010;</xsl:text>
        <xsl:text>    int arg;&#010;</xsl:text>
        <xsl:text>    trcS("initCmb\n");&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>    for(cmbKey= cmbBeg+1; cmbKey&lt;cmbEnd; cmbKey++){&#010;</xsl:text>
        <xsl:text>        idx= cmbKey-cmbBeg-1;&#010;</xsl:text>
        <xsl:text>        memset(&amp;loc[0],0,sizeof(tLocator)*3);&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>        arg= cmbTable[idx].arg0;&#010;</xsl:text>
        <xsl:text>        if(arg) getLocator(&amp;loc[0],arg);&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>        arg= cmbTable[idx].arg1;&#010;</xsl:text>
        <xsl:text>        if(arg) getLocator(&amp;loc[1],arg);&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>        arg= cmbTable[idx].arg2;&#010;</xsl:text>
        <xsl:text>        if(arg) getLocator(&amp;loc[2],arg);&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>        opr= cmbTable[idx].opr;&#010;</xsl:text>
        <xsl:text>        len= cmbTable[idx].len;&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>        ret= cmbInit(&amp;cmb[idx],opr,loc,len);&#010;</xsl:text>
        <xsl:text>        CHECK(ret>0,lblKO);&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
        <xsl:text>        ret= cntInit(&amp;cntCmb[idx],'x',&amp;cmb[idx]);&#010;</xsl:text>
        <xsl:text>        CHECK(ret>0,lblKO);&#010;</xsl:text>
        <xsl:text>    }&#010;</xsl:text>
        <xsl:text>    ret= 1;&#010;</xsl:text>
        <xsl:text>    goto lblEnd;&#010;</xsl:text>
        <xsl:text>lblKO:&#010;</xsl:text>
        <xsl:text>    trcErr(ret);&#010;</xsl:text>
        <xsl:text>    ret= -1;&#010;</xsl:text>
        <xsl:text>lblEnd:&#010;</xsl:text>
        <xsl:text>    return ret;&#010;</xsl:text>
        <xsl:text>}&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>

        <!-- Build mapCmbGet() function -->
        <xsl:text>static int mapCmbGet(word key,void *ptr,word len){&#010;</xsl:text>
        <xsl:text>    int ret;&#010;</xsl:text>
        <xsl:text>    ret= cmbGet(&amp;cmb[key],NULLKEY,ptr,len);&#010;</xsl:text>
        <xsl:text>    return ret;&#010;</xsl:text>
        <xsl:text>}&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="begKey"/>
<xsl:template match="uta:keySpace" mode="begKey">
  <xsl:apply-templates mode="begKey"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="begKey">
  <xsl:apply-templates mode="begKey"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="begKey">
  <xsl:apply-templates mode="begKey"/>
</xsl:template>

<xsl:template match="uta:derivated" mode="begKey">
  <xsl:apply-templates mode="begKey"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="begKey"/>

<xsl:template match="uta:record|uta:table|uta:combos|uta:converters|uta:find" mode="begKey">
  <xsl:text>    if(isSorted(</xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>Beg,key,</xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>End)) return </xsl:text>
  <xsl:value-of select="@uta:id"/>
  <xsl:text>Beg;</xsl:text>
  <xsl:if test="@uta:comment!=''">
    <xsl:text> //</xsl:text>
    <xsl:value-of select="@uta:comment"/>
  </xsl:if>
  <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="getContainer"/>
<xsl:template match="uta:keySpace" mode="getContainer">
    <xsl:apply-templates mode="getContainer"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="getContainer">
    <xsl:apply-templates mode="getContainer"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="getContainer">
    <xsl:apply-templates mode="getContainer"/>
</xsl:template>

<xsl:template match="uta:derivated" mode="getContainer">
    <xsl:apply-templates mode="getContainer"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="getContainer"/>

<xsl:template match="uta:record|uta:table" mode="getContainer">
        <!-- case appBeg: return &cntApp; -->
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return &amp;cnt</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos|uta:converters" mode="getContainer">
        <!-- case cmbBeg: return &cntCmb[relKey(key)]; -->
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return &amp;cnt</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>[relKey(key)];&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:keySpace" mode="mapInit">
    <xsl:apply-templates mode="mapInit"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="mapInit"/>
<xsl:template match="uta:find" mode="mapInit"/>

<xsl:template match="uta:dfsFile" mode="mapInit">
    <xsl:text>    //</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>
    <xsl:text>    ofs= 0;&#010;</xsl:text>
    <xsl:text>    sec= 0x</xsl:text>
    <xsl:value-of select="substring(@uta:id,7,2)"/>
    <xsl:text>;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:apply-templates mode="mapInit"/>
    <xsl:text>    trcFN("section </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text> size: %d\n",ofs);&#010;</xsl:text>
    <xsl:text>    strcpy(TlmFile[sec].ucName, "</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>");&#010;</xsl:text>
    <xsl:text>    TlmFile[sec].ulSize = ofs;&#010;</xsl:text>
    <xsl:text>    tot+= ofs;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapInit"/>
<xsl:template match="uta:volatile" mode="mapInit">
    <xsl:text>    //volatile memory&#010;</xsl:text>
    <xsl:text>    dbaSetPtr(&amp;vol);&#010;</xsl:text>
    <xsl:text>    ofs= 0;&#010;</xsl:text>
    <xsl:text>    sec= 0xFF;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
    <xsl:apply-templates mode="mapInit"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:record" mode="mapInit">
    <xsl:text>    //key subspace:</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>
        
    <xsl:text>    ret= recInit(&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,sec,ofs,DIM(</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>End),</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Len,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Fmt,</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Map);&#010;</xsl:text>
        
    <xsl:if test="@uta:dim!=''">
        <xsl:text>    len= ret; //len is used to initiate table&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
                
        <xsl:text>    ret= tabInit(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,sec,ofs,len,</xsl:text>
        <xsl:value-of select="@uta:dim"/>
        <xsl:text>,'R');&#010;</xsl:text> <!-- 'R': row is a record -->

    </xsl:if>
    
    <xsl:if test="@uta:multi!=''">
        <xsl:text>    len= ret; //len is used to initiate table&#010;</xsl:text>
        <xsl:text>&#010;</xsl:text>
                
        <xsl:text>    ret= tabInit(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,sec,ofs,len,DIM(</xsl:text>
        <xsl:value-of select="@uta:multi"/>
        <xsl:text>Beg,</xsl:text>
        <xsl:value-of select="@uta:multi"/>
        <xsl:text>End),'R');&#010;</xsl:text> <!-- 'R': row is a record -->
                
    </xsl:if>
    
    <xsl:text>    ofs+= ret;&#010;</xsl:text>
        
    <xsl:text>    ret= cntInit(&amp;cnt</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,'r',&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>); CHECK(ret>=0,lblKO);&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>

</xsl:template>

<xsl:template match="uta:table" mode="mapInit">
    <xsl:text>    //key subspace:</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>&#010;</xsl:text>

    <xsl:text>    ret= tabInit(&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,sec,ofs,</xsl:text>
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
    <xsl:text>,</xsl:text>
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
    <xsl:text>,</xsl:text>
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
    <xsl:text>);&#010;</xsl:text>
    
    
    <xsl:text>    ofs+= ret;&#010;</xsl:text>

    <xsl:text>    ret= cntInit(&amp;cnt</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,'t',&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>); &#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapInit">
    <xsl:apply-templates mode="mapInit"/>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:converters" mode="mapInit">
    <xsl:text>    ret= initCvt(); CHECK(ret>=0,lblKO); //init converters array&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos" mode="mapInit">
    <xsl:text>    ret= initCmb(); CHECK(ret>=0,lblKO); //init combos array&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:find" mode="mapInit">
    <xsl:apply-templates mode="mapInit"/>
</xsl:template>

<xsl:template match="uta:search" mode="mapInit">
    <xsl:text>    ret= searchInit(&amp;srh</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:where,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:where,2)"/>
    <xsl:text>,&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:where,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:where,2)"/>
    <xsl:text>,getContainer(</xsl:text>
    <xsl:value-of select="@uta:what"/>
    <xsl:text>)); CHECK(ret>=0,lblKO);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:index" mode="mapInit">
    <xsl:text>    ret= idxInit(&amp;idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,sec,ofs,&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:where,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:where,2)"/>
    <xsl:text>,&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:where,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:where,2)"/>
    <xsl:text>,getContainer(</xsl:text>
    <xsl:value-of select="@uta:what"/>
    <xsl:text>),</xsl:text>
    <xsl:text>idxNum</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,</xsl:text>
    <xsl:choose>
        <xsl:when test="@uta:length!=''">
            <xsl:value-of select="@uta:length"/>
            <xsl:text>,</xsl:text>
            <xsl:text>idxKey</xsl:text>
            <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(@uta:id,2)"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>0,0</xsl:text>
            <xsl:value-of select="@uta:type"/>
            <xsl:text></xsl:text>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text>); CHECK(ret>=0,lblKO);&#010;</xsl:text>

    <xsl:text>    trcFN("</xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text> index size: %d\n",ret);&#010;</xsl:text>
    <xsl:text>    ofs+= ret;&#010;</xsl:text>
    <xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapReset1"/>
<xsl:template match="uta:keySpace" mode="mapReset1">
    <xsl:apply-templates mode="mapReset1"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="mapReset1">
    <xsl:apply-templates mode="mapReset1"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapReset1">
    <xsl:apply-templates mode="mapReset1"/>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapReset1"/>
<xsl:template match="uta:rptLayout" mode="mapReset1"/>

<xsl:template match="uta:record" mode="mapReset1">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: ret= </xsl:text>
    <xsl:choose>
        <xsl:when test="@uta:dim!='' or @uta:multi!=''">
                        <xsl:text>tabReset(&amp;tab</xsl:text>
            <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(@uta:id,2)"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>recReset(&amp;rec</xsl:text>
            <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
            <xsl:value-of select="substring(@uta:id,2)"/>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:text>); break;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:table" mode="mapReset1">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: ret= tabReset(&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>); break;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:keySpace" mode="mapResetAll">
    <xsl:apply-templates mode="mapResetAll"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="mapResetAll">
    <xsl:apply-templates mode="mapResetAll"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapResetAll">
    <xsl:apply-templates mode="mapResetAll"/>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapResetAll"/>
<xsl:template match="uta:rptLayout" mode="mapResetAll"/>

<xsl:template match="uta:record" mode="mapResetAll">
    <xsl:choose>
        <xsl:when test="../@uta:reset='no'"/>
        <xsl:otherwise>
          <xsl:choose>
              <xsl:when test="@uta:dim!='' or @uta:multi!=''">
                  <xsl:text>        ret= tabReset(&amp;tab</xsl:text>
                  <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                  <xsl:value-of select="substring(@uta:id,2)"/>
              </xsl:when>
              <xsl:otherwise>
                  <xsl:text>        ret= recReset(&amp;rec</xsl:text>
                  <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
                  <xsl:value-of select="substring(@uta:id,2)"/>
              </xsl:otherwise>
          </xsl:choose>
          <xsl:text>); CHECK(ret>0,lblKO);&#010;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapResetAll"/>
<xsl:template match="uta:table" mode="mapResetAll">
    <xsl:choose>
        <xsl:when test="../@uta:reset='no'"/>
        <xsl:otherwise>
          <xsl:text>        ret= tabReset(&amp;tab</xsl:text>
          <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
          <xsl:value-of select="substring(@uta:id,2)"/>
          <xsl:text>); CHECK(ret>0,lblKO);&#010;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="uta:keySpace" mode="mapGet">
    <xsl:apply-templates mode="mapGet"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="mapGet">
    <xsl:apply-templates mode="mapGet"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapGet">
    <xsl:apply-templates mode="mapGet"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="mapGet"/>

<xsl:template match="uta:record" mode="mapGet">
        <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return mapRecGet(&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapGet"/>
<xsl:template match="uta:table" mode="mapGet">
        <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return mapTabGet(&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapGet">
    <xsl:apply-templates mode="mapGet"/>
</xsl:template>

<xsl:template match="uta:converters" mode="mapGet">
        <xsl:text>        case cvtBeg:  return mapCvtGet(key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos" mode="mapGet">
        <xsl:text>        case cmbBeg:  return mapCmbGet(key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:keySpace" mode="mapPut">
    <xsl:apply-templates mode="mapPut"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="mapPut">
    <xsl:apply-templates mode="mapPut"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapPut">
    <xsl:apply-templates mode="mapPut"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="mapPut"/>

<xsl:template match="uta:record" mode="mapPut">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return mapRecPut(&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:table" mode="mapPut">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return mapTabPut(&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapPut">
    <xsl:apply-templates mode="mapPut"/>
</xsl:template>

<xsl:template match="uta:converters" mode="mapPut">
        <xsl:text>        case cvtBeg: return mapCvtPut(key,ptr,len);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapPut"/>
<xsl:template match="uta:combos" mode="mapPut"/>

<xsl:template match="uta:keySpace" mode="mapDatLen">
    <xsl:apply-templates mode="mapDatLen"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="mapDatLen">
    <xsl:apply-templates mode="mapDatLen"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapDatLen">
    <xsl:apply-templates mode="mapDatLen"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="mapDatLen"/>

<xsl:template match="uta:record" mode="mapDatLen">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>.len[key];&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapDatLen"/>
<xsl:template match="uta:table" mode="mapDatLen">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return tabWdt(&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapDatLen">
    <xsl:apply-templates mode="mapDatLen"/>
</xsl:template>

<xsl:template match="uta:converters" mode="mapDatLen">
        <xsl:text>        case cvtBeg:  return cntLen(cnt,0);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos" mode="mapDatLen">
        <xsl:text>        case cmbBeg:  return cntLen(cnt,0);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:keySpace" mode="mapDatFmt">
    <xsl:apply-templates mode="mapDatFmt"/>
</xsl:template>

<xsl:template match="uta:dfsFile" mode="mapDatFmt">
    <xsl:apply-templates mode="mapDatFmt"/>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapDatFmt">
    <xsl:apply-templates mode="mapDatFmt"/>
</xsl:template>

<xsl:template match="uta:rptLayout" mode="mapDatFmt"/>

<xsl:template match="uta:record" mode="mapDatFmt">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return recFldFmt(&amp;rec</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,key);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapDatFmt"/>
<xsl:template match="uta:table" mode="mapDatFmt">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg: return tabRowFmt(&amp;tab</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapDatFmt">
    <xsl:apply-templates mode="mapDatFmt"/>
</xsl:template>

<xsl:template match="uta:converters" mode="mapDatFmt">
        <xsl:text>        case cvtBeg:  return 'C';&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:combos" mode="mapDatFmt">
        <xsl:text>        case cmbBeg:  return 'X';&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapMove"/>
<xsl:template match="uta:keySpace" mode="mapMove">
    <xsl:apply-templates mode="mapMove"/>
</xsl:template>

<!-- BKC_TEAM >>> -->  
<xsl:template match="uta:volatile" mode="mapMove">
    <xsl:apply-templates mode="mapMove"/>
</xsl:template>
<!-- BKC_TEAM<<< -->

<xsl:template match="uta:dfsFile" mode="mapMove">
    <xsl:apply-templates mode="mapMove"/>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapMove"/>
<xsl:template match="uta:rptLayout" mode="mapMove"/>

<xsl:template match="uta:record" mode="mapMove">
    <xsl:if test="@uta:dim!='' or @uta:multi!=''">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: tab= &amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>; rec= &amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>; break;</xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapCache"/>
<xsl:template match="uta:derivated" mode="mapCache"/>
<xsl:template match="uta:rptLayout" mode="mapCache"/>

<xsl:template match="uta:record" mode="mapCache">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg:&#010;</xsl:text>
        <xsl:text>            if(key>0)&#010;</xsl:text>
        <xsl:text>                return recCache(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Cache,sizeof(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Cache));&#010;</xsl:text>
        <xsl:text>            return recCache(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,0,0);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapCache"/>
<xsl:template match="uta:table" mode="mapCache">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg:&#010;</xsl:text>
        <xsl:text>            if(key>0)&#010;</xsl:text>
        <xsl:text>                return tabCache(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Cache,sizeof(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Cache));&#010;</xsl:text>
        <xsl:text>            return tabCache(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>,0,0);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapFlush"/>
<xsl:template match="uta:volatile" mode="mapFlush"/>
<xsl:template match="uta:derivated" mode="mapFlush"/>
<xsl:template match="uta:rptLayout" mode="mapFlush"/>

<xsl:template match="uta:record" mode="mapFlush">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return recFlush(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:table" mode="mapFlush">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return tabFlush(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapLoad"/>
<xsl:template match="uta:derivated" mode="mapLoad"/>
<xsl:template match="uta:rptLayout" mode="mapLoad"/>

<xsl:template match="uta:record" mode="mapLoad">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return recLoad(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:table" mode="mapLoad">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return tabLoad(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:find" mode="mapLoad">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg:&#010;</xsl:text>
    <xsl:text>            switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapLoad"/>
    <xsl:text>                default: break;;&#010;</xsl:text>
    <xsl:text>                  };&#010;</xsl:text>
    <xsl:text>                  break;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapLoad"/>
<xsl:template match="uta:index" mode="mapLoad">
    <xsl:text>                case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>: return idxLoad(&amp;idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapSave"/>
<xsl:template match="uta:derivated" mode="mapSave"/>
<xsl:template match="uta:rptLayout" mode="mapSave"/>
<xsl:template match="uta:find" mode="mapSave">
    <xsl:apply-templates mode="mapSave"/>
</xsl:template>

<xsl:template match="uta:record" mode="mapSave">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return recSave(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapSave"/>
<xsl:template match="uta:table" mode="mapSave">
    <xsl:if test="@uta:cache='yes'">
        <xsl:text>        case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: return tabSave(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>);&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:find" mode="mapSave">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>Beg:&#010;</xsl:text>
    <xsl:text>            switch(key){&#010;</xsl:text>
    <xsl:apply-templates mode="mapSave"/>
    <xsl:text>                default: break;;&#010;</xsl:text>
    <xsl:text>                  };&#010;</xsl:text>
    <xsl:text>                  break;&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:index" mode="mapSave">
    <xsl:text>                case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>: return idxSave(&amp;idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapExport"/>
<xsl:template match="uta:row" mode="mapExport">
    <xsl:if test="@uta:export!=''">
        <xsl:text>                case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>: return tabExport(&amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(../@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(../@uta:id,2)"/>
        <xsl:text>,rel,sec,'</xsl:text>
        <xsl:value-of select="@uta:export"/>
        <xsl:text>',"</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>");&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:record" mode="mapExport">
	<xsl:choose>
		<xsl:when test="@uta:dim!=''">
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates mode="mapExport"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="uta:field" mode="mapExport">
    <xsl:if test="@uta:export!=''">
        <xsl:text>                case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>: return recExport(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(../@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(../@uta:id,2)"/>
        <xsl:text>,rel,sec,'</xsl:text>
        <xsl:value-of select="@uta:export"/>
        <xsl:text>',"</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>");&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapExport"/>
<xsl:template match="uta:rptLayout" mode="mapExport"/>

<xsl:template match="uta:table" mode="mapExportGroup">
    <xsl:if test="@uta:export!=''">
        <xsl:text>                case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg:&#010;</xsl:text>
        <xsl:text>                    tab= &amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>;&#010;</xsl:text>
        <xsl:text>                    beg= </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg;&#010;</xsl:text>
        <xsl:text>                    end= </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>End;&#010;</xsl:text>
        <xsl:text>                    str= "</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>";&#010;</xsl:text>
        <xsl:text>                    exp= '</xsl:text>
        <xsl:value-of select="@uta:export"/>
        <xsl:text>';&#010;</xsl:text>
        <xsl:text>                    break;&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapExportGroup"/>
<xsl:template match="uta:rptLayout" mode="mapExportGroup"/>
<xsl:template match="uta:dbk" mode="mapExportGroup"/>

<xsl:template match="uta:record" mode="mapGetRecDim">
    <xsl:if test="@uta:dim!=''">
        <xsl:text>            case </xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>Beg: tab= &amp;tab</xsl:text>
        <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(@uta:id,2)"/>
        <xsl:text>; break;</xsl:text>
        <xsl:text>&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapGetRecDim"/>
<xsl:template match="uta:rptLayout" mode="mapGetRecDim"/>
<xsl:template match="uta:dbk" mode="mapGetRecDim"/>

<xsl:template match="uta:record" mode="mapExportDim">
	<xsl:choose>
		<xsl:when test="@uta:dim!=''">
			<xsl:apply-templates mode="mapExportDim"/>
		</xsl:when>
		<xsl:otherwise>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="uta:field" mode="mapExportDim">
    <xsl:if test="@uta:export!=''">
        <xsl:text>            if (key == begKey(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>)){ char buf[256];&#010;</xsl:text>
        <xsl:text>                sprintf(buf,"Index: %d - %s",idx,"</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>");&#010;</xsl:text>
        <xsl:text>                ret= recExport(&amp;rec</xsl:text>
        <xsl:value-of select="translate(substring(../@uta:id,1,1),$az,$AZ)"/>
        <xsl:value-of select="substring(../@uta:id,2)"/>
        <xsl:text>,relKey(</xsl:text>
        <xsl:value-of select="@uta:id"/>
        <xsl:text>),sec,'</xsl:text>
        <xsl:value-of select="@uta:export"/>
        <xsl:text>',buf); CHECK(ret&#62;=0,lblKO);&#010;</xsl:text>
        <xsl:text>            }&#010;</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template match="uta:derivated" mode="mapExportDim"/>
<xsl:template match="uta:rptLayout" mode="mapExportDim"/>
<xsl:template match="uta:dbk" mode="mapExportDim"/>

<xsl:template match="uta:dbk" mode="mapFind"/>
<xsl:template match="uta:keySpace" mode="mapFind"><xsl:apply-templates mode="mapFind"/></xsl:template>
<xsl:template match="uta:dfsFile" mode="mapFind"><xsl:apply-templates mode="mapFind"/></xsl:template>
<xsl:template match="uta:find" mode="mapFind"><xsl:apply-templates mode="mapFind"/></xsl:template>
<xsl:template match="uta:volatile" mode="mapFind"/>
<xsl:template match="uta:derivated" mode="mapFind"><xsl:apply-templates mode="mapFind"/></xsl:template>
<xsl:template match="uta:converters" mode="mapFind"/>
<xsl:template match="uta:combos" mode="mapFind"/>
<xsl:template match="uta:rptLayout" mode="mapFind"/>
<xsl:template match="uta:record" mode="mapFind"/>
<xsl:template match="uta:table" mode="mapFind"/>

<xsl:template match="uta:search" mode="mapFind">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>: return </xsl:text>
    <xsl:value-of select="@uta:how"/>
    <xsl:text>Find(&amp;srh</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,pat,beg,end);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:index" mode="mapFind">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>: return </xsl:text>
    <xsl:text>idxFind(&amp;idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>,pat,beg,end);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:volatile" mode="mapIns"/>
<xsl:template match="uta:derivated" mode="mapIns"/>
<xsl:template match="uta:rptLayout" mode="mapIns"/>

<xsl:template match="uta:dbk" mode="mapIns"/>
<xsl:template match="uta:index" mode="mapIns">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>: return idxIns(&amp;idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>);&#010;</xsl:text>
</xsl:template>

<xsl:template match="uta:dbk" mode="mapDel"/>
<xsl:template match="uta:volatile" mode="mapDel"/>
<xsl:template match="uta:derivated" mode="mapDel"/>
<xsl:template match="uta:rptLayout" mode="mapDel"/>

<xsl:template match="uta:index" mode="mapDel">
    <xsl:text>        case </xsl:text>
    <xsl:value-of select="@uta:id"/>
    <xsl:text>: return idxDel(&amp;idx</xsl:text>
    <xsl:value-of select="translate(substring(@uta:id,1,1),$az,$AZ)"/>
    <xsl:value-of select="substring(@uta:id,2)"/>
    <xsl:text>);&#010;</xsl:text>
</xsl:template>

</xsl:transform>
