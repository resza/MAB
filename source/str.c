//Subversion reference
//$HeadURL: https://svn.ingenico.com/UTAPROJECTS/sea-partner/5W/trunk/ISO5WPH/str/str.c $
//$Id: str.c 1917 2009-04-22 13:24:29Z abarantsev $


#include "global.h"
#include "str.h"
#include "emv.h"


/*  get date & time from logDatTim and format it for printing into traFmtDate
 * \header str\\str.h
 * \source str\\str.c
 */
int strFormatDatTim(word key) {
    int ret;
    card month;
    char datetime[lenDatTim + 1];
    char newdatetime[lenDatTim + 1];
    char fmtdatetime[lenDatTim + 1];
    const char *bgd = " ";
    const char *ctl = "ghabcdijkl";

    memset(newdatetime, 0, sizeof(newdatetime));
    memset(fmtdatetime, 0, sizeof(fmtdatetime));
    MAPGET(key, datetime, lblKO);
    trcFS("datetime: %s\n", datetime);

    dec2num(&month, &datetime[4], 2);
    ret = fmtMut(newdatetime, (const char *) datetime, bgd, ctl);

    CHECK(month <= 12, lblKO);
    CHECK(month > 0, lblKO);

    MAPGET((word)(monSpr + month), fmtdatetime, lblKO);
    memcpy(&fmtdatetime[3], newdatetime, strlen(newdatetime));
    trcFS("fmtdatetime: %s\n", fmtdatetime);
    memcpy(fmtdatetime+13,&datetime[12],2);
    MAPPUTSTR(traFmtDate, fmtdatetime, lblKO);
    CHECK(ret > 0, lblKO);
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    return ret;
}


int traReset(void) {
    int ret;
    char datetime[lenDatTim + 1];

    trcS("traReset: Beg\n");

    ret = mapReset(traBeg);
    CHECK(ret > 0, lblKO);

    memset(datetime, 0, sizeof(datetime));
    GetDateTime(datetime);    //CC+YYMMDDhhmmss
    
    ret = mapPut(traDatTim, datetime, lenDatTim);
    CHECK(ret > 0, lblKO);

    fmtAppNameAndVersion();

    ret = 1;
    goto lblEnd;
  lblKO:
    ret = -1;
  lblEnd:
    trcFN("traReset: ret=%d\n", ret);
    return ret;
}

/** Increments the passed parameter type entry with the max value of 999999
 * \header log\\log.h
 * \source log\\pmt.c
 */
int incCard(word key) {
    int ret;
    card buffer;

    trcS("Beg incCard \n");

    MAPGETCARD(key, buffer, lblKO);
    if(buffer >= 999999)
        buffer = 0;
    buffer++;
    MAPPUTCARD(key, buffer, lblKO);
    goto lblEnd;

  lblKO:
    ret = -1;
    goto lblEnd;
  lblEnd:
    trcFN("incCard ret=%d\n", ret);
    return ret;
}
