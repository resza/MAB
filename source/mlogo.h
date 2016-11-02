
/****************************************************************************
NAME
    mlogo.h - defines multiple logos and processing functions

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    Gary.Ho     2010.01.26      - created
****************************************************************************/

#ifndef _MLOGO_H
#define _MLOGO_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int GetNowDispLogo(unsigned char **ppsData);
int GetNowPrnLogo(unsigned char **ppsData);
int GetLogoWidthHeigh(unsigned char *psData, int *piWidth, int *piHeigh);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _MLOGO_H

// end of file
