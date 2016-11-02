#include "Farsi.h"
#include "fstruct.h"
#include <posapi.h>

//int     k_FileDispID, k_FilePrnID;
int     k_ArFileID;
int     k_ArInitFlag    = 0;
uchar   k_ArNumType     = 0;
uchar   k_ArExBracket   = 1;
uint    k_ArFileDispOffset, k_ArFilePrnOffset;

tFontIndex  k_FPrn[256];
tFontIndex  k_FDisp[256];

ushort  k_ArWordPos[1024];
uchar   k_ArShowLine;
ushort  k_ArLineWordWidth;
//==================================================================
void s_ShiftStr(uchar *OldStr, uchar *ShiftStr)
{
    uint    i, j;

    i = 0;
    j = 0;
    while(i < 1024)
    {
        if(OldStr[i] < 0x20)    return;
        switch(OldStr[i])
        {
        case '(':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = ')';
            break;
        case ')':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = '(';
            break;
        case '<':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = '>';
            break;
        case '>':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = '<';
            break;
        case '[':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = ']';
            break;
        case ']':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = '[';
            break;
        case '{':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = '}';
            break;
        case '}':
            if((k_FPrn[OldStr[i+1]].type==TYPE_RHYME) || (k_ArExBracket!=1))
                ShiftStr[j] = OldStr[i];
            else
                ShiftStr[j] = '{';
            break;
        case 0xE1:
            if(OldStr[i+1] == 0xC2)
            {
                ShiftStr[j] = 0x01;
                i++;
            }
            else if(OldStr[i+1] == 0xC3)
            {
                ShiftStr[j] = 0x02;
                i++;
            }
            else if(OldStr[i+1] == 0xC5)
            {
                ShiftStr[j] = 0x03;
                i++;
            }
            else if(OldStr[i+1] == 0xC7)
            {
                ShiftStr[j] = 0x04;
                i++;
            }
            else
            {
                ShiftStr[j] = OldStr[i];
            }
            break;
        default :
            ShiftStr[j] = OldStr[i];
            break;
        }
        i++;
        j++;
    }
}

uchar   s_ArGetForm(uchar code, uchar BType, uchar AType)
{
    uchar   CType;

    CType = k_FPrn[code].type;
    if(CType == TYPE_CONNECTOR)
    {
        if(BType == TYPE_CONNECTOR)
        {
            if(AType==TYPE_CONNECTOR || AType==TYPE_NONCONNECTOR)
                return(FORM_MIDDLE);
            else
                return(FORM_TERMINAL);
        }
        else
        {
            if(AType==TYPE_CONNECTOR || AType==TYPE_NONCONNECTOR)
                return(FORM_PREFIX);
            else
                return(FORM_INDEPEND);
        }
    }
    else if(CType == TYPE_NONCONNECTOR)
    {
        if(BType == TYPE_CONNECTOR)
            return(FORM_TERMINAL);
        else
            return(FORM_INDEPEND);
    }
    else if(CType == TYPE_NUMERIC)
    {
        if(k_ArNumType == 1)
            return(FORM_TERMINAL);
        else
            return(FORM_INDEPEND);
    }
    else if(CType == TYPE_RHYME)
    {
        if(BType==TYPE_CHAR || BType==TYPE_CONNECTOR || BType==TYPE_NONCONNECTOR || BType==TYPE_RHYME)
            return(FORM_INDEPEND | 0x80);
        else
            return(FORM_INDEPEND);
    }
    else
    {
        return(FORM_INDEPEND);
    }
}

uint    s_ArFindRhyme(uchar *buf, uint begin)
{
    uint    j;

    for(j=begin; j<strlen(buf); j++)
    {
        if(k_FPrn[buf[j]].type != TYPE_RHYME)
            break;
    }
    return(j);
}

uint    s_ArFindArWord(uchar *buf, uint begin)
{
    uint    j;
    uchar   CType;

    for(j=begin; j<strlen(buf); j++)
    {
        CType = k_FPrn[buf[j]].type;
        if(CType==TYPE_CONNECTOR || CType==TYPE_NONCONNECTOR || CType==TYPE_RHYME)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return(j);
}

uint    s_ArFindNum(uchar *buf, uint begin)
{
    uint    j;
    uchar   CType;

    for(j=begin; j<strlen(buf); j++)
    {
        CType = k_FPrn[buf[j]].type;
        if(CType == TYPE_NUMERIC)
            continue;
        else if(CType == TYPE_INTER_NUM)
        {
            if(k_FPrn[buf[j+1]].type == TYPE_NUMERIC)
                continue;
            else
                break;
        }
        else if(CType == TYPE_RHYME)
        {
            continue;
        }
        else if(CType == TYPE_EXP)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return(j);
}

uint    s_ArFindWord(uchar *buf, uint begin)
{
    uint    j;
    uchar   CType;

    for(j=begin; j<strlen(buf); j++)
    {
        CType = k_FPrn[buf[j]].type;
        if(CType == TYPE_CHAR)
            continue;
        else
        {
            break;
        }
    }
    return(j);
}

void s_ArFmtStr(uchar *ShiftStr, uchar *FmtStr)
{
    uint    i, j, k, WordPoint, rIndex, wIndex;
    uchar   BType, CType, AType, cForm;
    //uchar   isInterNum;

    rIndex = 0;
    wIndex = 0;
    BType  = TYPE_NONE;
    memset(&k_ArWordPos, 0x00, sizeof(k_ArWordPos));
    WordPoint = 0;
    while(1)
    {
        CType = k_FPrn[ShiftStr[rIndex]].type;
        if(CType == TYPE_NONE)   break;

//        if((CType==TYPE_CONNECTOR) || (CType==TYPE_NONCONNECTOR))
//        {
//            i     = s_ArFindRhyme(ShiftStr, rIndex+1);
//            AType = k_FPrn[ShiftStr[i]].type;
//            cForm = s_ArGetForm(ShiftStr[rIndex], BType, AType);
//            FmtStr[wIndex*2]     = ShiftStr[rIndex];
//            FmtStr[wIndex*2 + 1] = cForm;
//            wIndex++;
//            rIndex++;
//            BType = CType;
//            for(j=rIndex; j<i; j++)
//            {
//                FmtStr[wIndex*2]     = ShiftStr[j];
//                FmtStr[wIndex*2 + 1] = FORM_INDEPEND | 0x80;
//                wIndex++;
//                rIndex++;
//            }
//            continue;
//        }

        k_ArWordPos[WordPoint++] = wIndex;

        if((CType==TYPE_CONNECTOR) || (CType==TYPE_NONCONNECTOR))
        {
            k = s_ArFindArWord(ShiftStr, rIndex+1);
            while(rIndex < k)
            {
                CType = k_FPrn[ShiftStr[rIndex]].type;
                i     = s_ArFindRhyme(ShiftStr, rIndex+1);
                AType = k_FPrn[ShiftStr[i]].type;
                cForm = s_ArGetForm(ShiftStr[rIndex], BType, AType);
                FmtStr[wIndex*2]     = ShiftStr[rIndex];
                FmtStr[wIndex*2 + 1] = cForm;
                wIndex++;
                rIndex++;
                BType = CType;
                for(j=rIndex; j<i; j++)
                {
                    FmtStr[wIndex*2]     = ShiftStr[j];
                    FmtStr[wIndex*2 + 1] = FORM_INDEPEND | 0x80;
                    wIndex++;
                    rIndex++;
                }
            }
            continue;
        }


        if(CType == TYPE_RHYME)
        {
            i     = s_ArFindRhyme(ShiftStr, rIndex+1);
            cForm = s_ArGetForm(ShiftStr[rIndex], BType, TYPE_NONE);
            FmtStr[wIndex*2]     = ShiftStr[rIndex];
            FmtStr[wIndex*2 + 1] = cForm;
            wIndex++;
            rIndex++;
            BType = CType;
            for(j=rIndex; j<i; j++)
            {
                FmtStr[wIndex*2]     = ShiftStr[j];
                FmtStr[wIndex*2 + 1] = FORM_INDEPEND | 0x80;
                wIndex++;
                rIndex++;
            }
            continue;
        }

        if((CType==TYPE_INTERPUNCTION) || (CType==TYPE_INTER_NUM))
        {
            i = s_ArFindRhyme(ShiftStr, rIndex+1);
            BType = CType;
            if(ShiftStr[rIndex] == 0x5C)
            {
                FmtStr[wIndex*2]     = ShiftStr[rIndex];
                FmtStr[wIndex*2 + 1] = FORM_INDEPEND;
                wIndex++;
                rIndex++;
                BType = TYPE_RHYME;
                continue;
            }
            for(j=i; j>rIndex; j--)
            {
                FmtStr[wIndex*2]     = ShiftStr[j-1];
                FmtStr[wIndex*2 + 1] = FORM_INDEPEND;
                wIndex++;
            }
            rIndex = i;
            continue;
        }

        if(CType == TYPE_CHAR)
        {
            cForm = FORM_INDEPEND;
            i     = s_ArFindWord(ShiftStr, rIndex+1);
            for(j=i; j>rIndex; j--)
            {
                FmtStr[wIndex*2]     = ShiftStr[j-1];
                FmtStr[wIndex*2 + 1] = cForm;
                wIndex++;
            }
            rIndex = i;
            BType  = CType;
            continue;
        }

        if(CType==TYPE_EXP || CType==TYPE_NUMERIC)
        {
            i = s_ArFindNum(ShiftStr, rIndex+1);
            for(j=i; j>rIndex; j--)
            {
                FmtStr[wIndex*2]     = ShiftStr[j-1];
                FmtStr[wIndex*2 + 1] = s_ArGetForm(ShiftStr[j-1], TYPE_NUMERIC, TYPE_NONE);
                wIndex++;
            }
            rIndex = i;
            BType  = TYPE_NUMERIC;
            continue;
        }
    }
}

uint s_ArFindLine(tFontIndex *Head, uchar *FmtStr, uint Index, uint BlankWidth)
{
    uint    i, j, k;
    uint    CurIndex, EndIndex;
    uint    WordWidth;
    uchar   code, status;

    CurIndex  = Index;
    EndIndex  = CurIndex;
    k_ArLineWordWidth = 0;
    for(i=0; i<1024; i++)
    {
        if(Head[FmtStr[2*CurIndex]].type == TYPE_NONE)
        {
            return(EndIndex);
        }
        WordWidth = 0;
        for(j=0; j<1024; j++)
        {
            code   = FmtStr[2*CurIndex];
            status = FmtStr[2*CurIndex + 1];
            if(Head[code].type == TYPE_NONE)  break;
            if(!(status & 0x80))
                WordWidth += Head[code].fdot[status&0x7F].width;
            if(WordWidth + k_ArLineWordWidth > BlankWidth)
            {
                if(i == 0)
                {
                    k_ArLineWordWidth += WordWidth - Head[code].fdot[status&0x7F].width;
                    return(CurIndex);
                }
                break;
            }
            CurIndex++;
            for(k=0; k<1024; k++)
            {
                if(k_ArWordPos[k] == CurIndex) break;
            }
            if(k < 1024)  break;
        }
        if(WordWidth + k_ArLineWordWidth > BlankWidth)
        {
//            if(i == 0)  return(CurIndex);
//            else    return(EndIndex);
            return(EndIndex);
        }
        EndIndex    = CurIndex;
        k_ArLineWordWidth += WordWidth;
//        BlankWidth -= WordWidth;
    }
	return (EndIndex);
}

uchar s_ArShowStrLine(uchar Mode, uchar *FmtStr, uint BegIndex, uint EndIndex)
{
    uchar   buf[4096], dot[1024];
    uint    fillx, filly, offset;
    //uint    fx;
    uchar   height, width;
    uint    index, i, j, iRet;
    uchar   ucRet, code, status;
    uchar   filloff, tempdot;
////////////////////
//uchar   data[1024];
//uint    x,y;
//PortOpen(0, "115200,8,n,1");

///////////////////
//seek(k_ArFileID, 0x6040, SEEK_SET);
//memset(dot, 0x00, sizeof(dot));
//read(k_ArFileID, dot, 64);
//for(x=0; x<32; x++)
//{
//    memset(data, 0x00, sizeof(data));
//    for(y=0; y<2; y++)
//    {
//        sprintf(data+strlen(data), "%02X ", dot[x*2 + y]);
//    }
//    sprintf(data+strlen(data), "\r\n");
//    PortSends(0, data, strlen(data));
//}
//getkey();
///////////////////
////////////////////

    if(k_FDisp[FmtStr[2*BegIndex]].type == TYPE_NONE)
        return(AR_SUCCESS);
    ucRet = Mode & (~EFT_REVERSE);
    if(ucRet == SHIFT_CENTER)
    {
        fillx = DISP_LINE_LEN - (DISP_LINE_LEN-k_ArLineWordWidth)/2;
    }
    else if(ucRet == SHIFT_LEFT)
    {
        fillx = k_ArLineWordWidth;
    }
    else
    {
        fillx = DISP_LINE_LEN;
    }
    filly = 0;
    index = BegIndex;
    memset(buf, 0x00, sizeof(buf));

    while(1)
    {
        if(index >= EndIndex)   break;
        code   = FmtStr[index*2];
        status = FmtStr[index*2+1];
        if(k_FDisp[code].type == TYPE_NONE)   break;
        height = k_FDisp[code].fdot[status&0x7f].height;
        width  = k_FDisp[code].fdot[status&0x7f].width;
        offset = k_FDisp[code].fdot[status&0x7f].offset;

        if(status & 0x80)   fillx += width;
        if(width > fillx)   break;
        if(filly < height)
            filly = height;

        iRet = seek(k_ArFileID, offset + k_ArFileDispOffset, SEEK_SET);
        if(iRet != 0)
        {
            return(AR_READFILE_ERROR);
        }
        memset(dot, 0x00, sizeof(dot));
        read(k_ArFileID, dot, (int)(((height+7)/8)*width));
/////////////////
//memset(data, 0x00, sizeof(data));
//sprintf(data, "code=%02xH\r\nstatus=%u\r\nheight=%u\r\nwidth=%u\r\noffset=%u\r\nread=%08xH\r\n",code,status,height,width,offset,offset + k_ArFileDispOffset);
//PortSends(0, data, strlen(data));
//
//for(x=0; x<(height+7)/8; x++)
//{
//    memset(data, 0x00, sizeof(data));
//    for(y=0; y<width; y++)
//    {
//        sprintf(data+strlen(data), "%02X ", dot[x*width + y]);
//    }
//    sprintf(data+strlen(data), "\r\n");
//    PortSends(0, data, strlen(data));
//}
//getkey();
/////////////////////

        filloff = fillx - width;
        //for(i=0; i < ((height+7)/8); i++)	//irf, 20121128
		for(i=0; i < (uint)((height+7)/8); i++)	//irf, 20121128
        {
            for(j=0; j<width; j++)
            {
                tempdot = dot[i*width + j];
                buf[1 + i*(DISP_LINE_LEN+2) + 2 + filloff + j] |= tempdot;
            }
        }
        index++;
        fillx -= width;
    }

    buf[0] = (uchar)((filly+7)/8);
    for(i=0; i<buf[0]; i++)
    {
        buf[1 + i*(DISP_LINE_LEN+2)]     = 0x00;
        buf[1 + i*(DISP_LINE_LEN+2) + 1] = DISP_LINE_LEN;
        if(Mode & EFT_REVERSE)
        {
            for(j=0; j<DISP_LINE_LEN; j++)
            {
                buf[1 + i*(DISP_LINE_LEN+2) + 2 + j] = ~buf[1 + i*(DISP_LINE_LEN+2) + 2 + j];
            }
        }
    }
///////////////////
//for(x=0; x<buf[0]; x++)
//{
//    memset(data, 0x00, sizeof(data));
//    for(y=0; y<DISP_LINE_LEN+2; y++)
//    {
//        sprintf(data+strlen(data), "%02X ", buf[1 + x*(DISP_LINE_LEN+2) + y]);
//    }
//    sprintf(data+strlen(data), "\r\n");
//    PortSends(0, data, strlen(data));
//}
//getkey();

///////////////////
    //ScrDrLogoxy(0, 8*lineno, buf);
    ScrGotoxy(0, k_ArShowLine);
    ScrDrLogo(buf);
    k_ArShowLine += buf[0];
    return(AR_SUCCESS);
}

uchar s_ArShowStr(uchar Mode, uchar *FmtStr)
{
    uint    BegIndex, EndIndex;
    uchar   ucRet;

    BegIndex = 0;
    while(1)
    {
        if(k_FDisp[FmtStr[2*BegIndex]].type == TYPE_NONE)    return(AR_SUCCESS);
        EndIndex = s_ArFindLine(k_FDisp, FmtStr, BegIndex, DISP_LINE_LEN);
        ucRet = s_ArShowStrLine(Mode, FmtStr, BegIndex, EndIndex);
        if(ucRet != AR_SUCCESS) return(ucRet);
        BegIndex = EndIndex;
    }
    return(AR_SUCCESS);
}

uchar s_ArPrnStrLine(uchar Mode, uchar *FmtStr, uint BegIndex, uint EndIndex)
{
    uchar   buf[4096], dot[1024];
    uint    fillx, filly, offset;
    uint    fx;
    uchar   height, width;
    uint    index, i, j, iRet;
    uchar   ucRet, code, status;
    uchar   filloff, tempdot;
////////////////////
//uchar   data[1024];
//uint    x,y;
//PortOpen(0, "115200,8,n,1");
//
//
///////////////////
//seek(k_ArFileID, 0x6040, SEEK_SET);
//memset(dot, 0x00, sizeof(dot));
//read(k_ArFileID, dot, 64);
//for(x=0; x<32; x++)
//{
//    memset(data, 0x00, sizeof(data));
//    for(y=0; y<2; y++)
//    {
//        sprintf(data+strlen(data), "%02X ", dot[x*2 + y]);
//    }
//    sprintf(data+strlen(data), "\r\n");
//    PortSends(0, data, strlen(data));
//}
//getkey();
///////////////////
////////////////////

    if(k_FPrn[FmtStr[2*BegIndex]].type == TYPE_NONE)
        return(AR_SUCCESS);
    ucRet = Mode & (~EFT_REVERSE);
    if(ucRet == SHIFT_CENTER)
    {
        fillx = PRN_LINE_LEN - (PRN_LINE_LEN-k_ArLineWordWidth)/2;
    }
    else if(ucRet == SHIFT_LEFT)
    {
        fillx = k_ArLineWordWidth;
    }
    else
    {
        fillx = PRN_LINE_LEN;
    }
    filly = 0;
    index = BegIndex;
    memset(buf, 0x00, sizeof(buf));

    while(1)
    {
        if(index >= EndIndex)   break;
        code   = FmtStr[index*2];
        status = FmtStr[index*2+1];
        if(k_FPrn[code].type == TYPE_NONE)   break;
        height = k_FPrn[code].fdot[status&0x7f].height;
        width  = k_FPrn[code].fdot[status&0x7f].width;
        offset = k_FPrn[code].fdot[status&0x7f].offset;

        if(status & 0x80)   fillx += width;
        if(width > fillx)   break;
        if(filly < height)
            filly = height;

        iRet = seek(k_ArFileID, offset + k_ArFilePrnOffset, SEEK_SET);
        if(iRet != 0)
        {
            return(AR_READFILE_ERROR);
        }
        memset(dot, 0x00, sizeof(dot));
        read(k_ArFileID, dot, (int)((width+7)/8*height));
/////////////////
//memset(data, 0x00, sizeof(data));
//sprintf(data, "code=%02xH\r\nstatus=%u\r\nheight=%u\r\nwidth=%u\r\noffset=%u\r\nread=%08xH\r\n",code,status,height,width,offset,offset + k_ArFilePrnOffset);
//PortSends(0, data, strlen(data));
//
//for(x=0; x<height; x++)
//{
//    memset(data, 0x00, sizeof(data));
//    for(y=0; y<(width+7)/8; y++)
//    {
//        sprintf(data+strlen(data), "%02X ", dot[x*(width+7)/8 + y]);
//    }
//    sprintf(data+strlen(data), "\r\n");
//    PortSends(0, data, strlen(data));
//}
//getkey();
/////////////////
        fx      = (fillx - width) / 8;
        filloff = (fillx - width) % 8;
        for(i=0; i<height; i++)
        {
            //for(j=0; j < ((width+7)/8); j++)	//irf, 20121128
			for(j=0; j < (uint)((width+7)/8); j++)	//irf, 20121128
            {
                tempdot = dot[i*((width+7)/8) + j];
                buf[1 + i*(PRN_LINE_LEN/8+2) + 2 + fx + j]     |= (tempdot>>filloff);
                buf[1 + i*(PRN_LINE_LEN/8+2) + 2 + fx + j + 1] |= (tempdot<<(8-filloff));
            }
        }
        index++;
        fillx -= width;
    }

    buf[0] = (uchar)filly;
    for(i=0; i<filly; i++)
    {
        buf[1 + i*(PRN_LINE_LEN/8+2)]     = 0x00;
        buf[1 + i*(PRN_LINE_LEN/8+2) + 1] = PRN_LINE_LEN/8;
        if(Mode & EFT_REVERSE)
        {
            for(j=0; j<PRN_LINE_LEN/8; j++)
            {
                buf[1 + i*(PRN_LINE_LEN/8+2) + 2 + j] = ~buf[1 + i*(PRN_LINE_LEN/8+2) + 2 + j];
            }
        }
    }
    ucRet = PrnLogo(buf);
    return(ucRet);
}

uchar s_ArPrnStr(uchar Mode, uchar *FmtStr)
{
    uint    BegIndex, EndIndex;
    uchar   ucRet;

    BegIndex = 0;
    while(1)
    {
        if(k_FPrn[FmtStr[2*BegIndex]].type == TYPE_NONE)    return(AR_SUCCESS);
        EndIndex = s_ArFindLine(k_FPrn, FmtStr, BegIndex, PRN_LINE_LEN);
        ucRet = s_ArPrnStrLine(Mode, FmtStr, BegIndex, EndIndex);
        if(ucRet != AR_SUCCESS) return(ucRet);
        BegIndex = EndIndex;
    }
    return(AR_SUCCESS);
}

//==================================================================
uchar ArInit(uchar PrnFontID)
{
    int     iRet;
    uchar   Ver[ARFILE_VER_LEN + 1];

    if(PrnFontID==0 || PrnFontID>20)
    {
        return(AR_PARA_ERROR);
    }
    if(k_ArInitFlag)
    {
        ArStop();
    }
    // Open Farsi font library
    k_ArFileID = open(ARFILE_NAME, O_RDWR);
    if(k_ArFileID == -1)
    {
        return(AR_OPENFILE_ERROR);
    }
    // Read the font library head
    iRet = seek(k_ArFileID, 0x0, SEEK_SET);
    if(iRet != 0)
    {
        return(AR_READFILE_ERROR);
    }
    memset(Ver, 0x00, sizeof(Ver));
    read(k_ArFileID, Ver, ARFILE_VER_LEN);

    k_ArFileDispOffset = 0;
    k_ArFilePrnOffset  = 0;
    iRet = seek(k_ArFileID, ARFILE_VER_LEN, SEEK_SET);
    if(iRet != 0)
    {
        return(AR_READFILE_ERROR);
    }
    read(k_ArFileID, (uchar *)&k_ArFileDispOffset, sizeof(uint));
    if(k_ArFileDispOffset == 0) return(AR_READFILE_ERROR);

    iRet = seek(k_ArFileID, ARFILE_VER_LEN + PrnFontID*sizeof(uint), SEEK_SET);
    if(iRet != 0)
    {
        return(AR_READFILE_ERROR);
    }
    read(k_ArFileID, (uchar *)&k_ArFilePrnOffset, sizeof(uint));
    if(k_ArFilePrnOffset == 0)  return(AR_PARA_ERROR);


    iRet = seek(k_ArFileID, k_ArFileDispOffset, SEEK_SET);
    if(iRet != 0)
    {
        return(AR_READFILE_ERROR);
    }
    read(k_ArFileID, (uchar *)&k_FDisp, sizeof(k_FDisp));

    iRet = seek(k_ArFileID, k_ArFilePrnOffset, SEEK_SET);
    if(iRet != 0)
    {
        return(AR_READFILE_ERROR);
    }
    read(k_ArFileID, (uchar *)&k_FPrn, sizeof(k_FPrn));
    k_ArInitFlag = 1;
    return(AR_SUCCESS);

}
uchar ArStop()
{
    // Close file id
    if(k_ArInitFlag)
    {
        close(k_ArFileID);
    }
    k_ArInitFlag = 0;
    return(AR_SUCCESS);
}
uchar ArShowString(uchar lineno, uchar Mode, uchar *str)
{
    uchar   TempStr[1025];
    uchar   ShiftStr[1025];
    uchar   FmtStr[2050];
    uchar   ucRet;
    uint    i, j;

    if(strlen(str) > 1024)  return(AR_PARA_ERROR);
    if(lineno > 7)          return(AR_PARA_ERROR);
    if(!k_ArInitFlag)
    {
        return(AR_READFILE_ERROR);
    }

    i = 0;
    k_ArShowLine = lineno;
    while(i < strlen(str))
    {
        memset(TempStr, 0x00, sizeof(TempStr));
        for(j=0; j<strlen(str); j++)
        {
            TempStr[j] = str[i++];
            if(TempStr[j]=='\n' || TempStr[j]==0x00)  break;
        }
        if(TempStr[0] == '\n')
        {
            k_ArShowLine += 2;
            continue;
        }
        memset(ShiftStr, 0x00, sizeof(ShiftStr));
        s_ShiftStr(TempStr, ShiftStr);
        memset(FmtStr, 0x00, sizeof(FmtStr));
        s_ArFmtStr(ShiftStr, FmtStr);
        ucRet = s_ArShowStr(Mode, FmtStr);
        if(ucRet != AR_SUCCESS)
            return(ucRet);
    }
    return(AR_SUCCESS);
}

uchar ArPrintString(uchar Mode, uchar *str)
{
    uchar   TempStr[1025];
    uchar   ShiftStr[1025];
    uchar   FmtStr[2050];
    uchar   ucRet;
    uint    i, j;

    if(strlen(str) > 1024)  return(AR_PARA_ERROR);
    if(!k_ArInitFlag)
    {
        return(AR_READFILE_ERROR);
    }

    i = 0;
    while(i < strlen(str))
    {
        memset(TempStr, 0x00, sizeof(TempStr));
        for(j=0; j<strlen(str); j++)
        {
            TempStr[j] = str[i++];
            if(TempStr[j]=='\n' || TempStr[j]==0x00)  break;
        }
        if(TempStr[0] == '\n')
        {
            ucRet = k_FPrn[32].fdot[0].height;
            PrnStep(ucRet);
            continue;
        }
        memset(ShiftStr, 0x00, sizeof(ShiftStr));
        s_ShiftStr(TempStr, ShiftStr);
        memset(FmtStr, 0x00, sizeof(FmtStr));
        s_ArFmtStr(ShiftStr, FmtStr);
        ucRet = s_ArPrnStr(Mode, FmtStr);
        if(ucRet != AR_SUCCESS)
            return(ucRet);
    }
    return(AR_SUCCESS);
}

uchar ArChgNumType(uchar type)
{
    k_ArNumType = (type == 0x00) ? 0x00:0x01;
    return k_ArNumType;
}

