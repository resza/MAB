/*********************************************************************************
 Copyright (C) 2007 XieLihong
 ��Ȩ����      2007 л���� 

        ���������֧������  mult-language support engine
����        л����
Email       xiliho221@163.com xielh@pazsz.com
��ǰ�汾    1.1
��ʷ��Ҫ
2008.01.12 V1.0.012 xlh
    �����Ҹ�ð�ˣ�˼·������Щ������������bug�����߶Դ��������Ľ��Ļ���ϣ�����Ը���email
    ��������˼·���޸ģ�����������汾��������������
	˼·��
	1. ��ȡ�ļ���fileBuf��
	2. �����ļ���ͬʱ��"\n"���ַ���ת��Ϊ��Ӧ��ascii�룬�����ַ�����������strIdx��
	3. ��strIdx���п������� (qsort)
	4. ����strIdx[i].src[0]����ascii����ascIdx (CreatAscIdx)
	   ��strIdx[i].src[0]Ϊ 'c' , 'c' ��ֵΪ99,������'c'��ͷ�Ķ���Ӧ��ascIdx[99]��
	   ascIdx[99].begin ��ʾ'c'��ͷ���ַ������ַ�������strIdx�еĿ�ʼλ��
	   ascIdx[99].count ��ʾ'c'��ͷ���ַ������ַ�������strIdx�еĸ���
	5.  ���ں���char *Translate(char *str);
	   ��printf(_T("china")); 
	   1> 'c'��ͷ���ַ� ��ascIdx�и���Ϊ0,ֱ�ӷ����ַ���
	   2> 'c'��ͷ���ַ� ��ascIdx�и���Ϊ1,ֱ�ӷ���begin��ָ���ַ��� //(��������ļ�����Ļ�����һ������)
	   3> 'c'��ͷ���ַ� ��ascIdx�и�������2,����begin��count,���ж��ֲ���(bsearch),
	      ����count��ֵ�������в��Һܿ죬һ�������αȽϾͿ���ƥ����ַ���

2008.01.13 V1.0.013 xlh
   1.�ı������ļ��ĸ�ʽ��ԭ���������ļ�����'='ʱ�е����⣬ͬʱ�ı������ļ��Ķ�ȡ�㷨
   2."ȥ��'c'��ͷ���ַ� ��ascIdx�и���Ϊ1,ֱ�ӷ���begin��ָ���ַ���"����Ϊ��������ļ�����Ļ����ᵼ�´���ķ���
   3.�Ľ����ļ������fileBuf�İ�ȫʹ���㷨��ԭ����ȫ����գ�����ֻ�����һ������'\0'
   �����hash��ʵ�ֲ��ң������ջ��Ƿ����ˣ��õ�hash�����������ң�һ��Ļ��������ڵ�"���������ֲ���"

2008.01.14 V1.0.0114 xlh
   1.��Ӷ� "\x64" "\x7" ת���ַ���֧��

2008.01.22 v1.0.0122 xlh
   1.���const char *GetLngName(void)�����ڼ���ⲿ���Ե�ʹ�����
   2.֧��UNIX��DOS���ֲ�ͬ���ļ���ʽ

2008.07.09 v1.0.0709 xlh
   ���� SetLng�ķ���ֵ��ʹ֮���°汾�ļ���
   ���� 0 ��ʾ�ɹ�
   <0 ��ʾʧ�ܣ��������ڲ�����

2009.08.08 v1.1.0807 xlh
   ��һ�汾�ڴ�����ַ����ַ���ʱ��������һ���ַ��ĵڶ����ֽ�Ϊ5c,��BIG5��"��"�֡�
   ����0x5c ���ַ�Ϊ '\\' ��͵����ַ�����󹹳��� \" �������ַ������Ӷ����½���ʱ����

   1. ��һ�汾�Ľ������Ϊ��(�µķ��������Ͽ��Խ�����е��ַ���)
       " SALE " = "��  ��"
       1> ����ͷ��ʼ��������һ������
       2> ���� " = " ������
         ��Ȩ�ķ�ʽ���ж�
         Ȩ = 0  : û��������
	     Ȩ = 1  : \" = " ��ϣ��� \" = " = "�ĵ�һ��
	     Ȩ = 2  : 0x80 0x5c " = " ��ϣ����� "Ȩ = 1" �Ļ����ϣ�'\' ǰ����ַ�����0x7f, Ϊ���ַ��������
	     Ȩ = 3  : " = " ��ϣ�������������
	       ��ע: ʵ������У����ܲ�����  "Ȩ = 1" ��"Ȩ = 2"����һ����ͬʱ���ڶ�Ӱ����������Դ����н�"Ȩ = 1" ��"Ȩ = 2ͳһΪ"Ȩ = 1" 
      3> ����ҵ� "Ȩ = 3" ���ҵ������λ�ã��Ҳ����Ļ���ȡ�ҵ�������Ȩ��

      4> ����β��ʼ���������һ�����š�

   2.  �����еĵ�һ���ַ�Ϊ '#' ʱ����һ��Ϊע��
   3.  SetLng ������������ֵ
      -5 MAX_LNG_FILE_SIZE ��С
	  -6 MAX_LNG_STR ��С
   4. ��˽�еı����ͺ�������static
   5. �Ӵ�Ĭ�� MAX_LNG_FILE_SIZE �� MAX_LNG_STR ��ֵ



**********************************************************************************/

#include "lng.h"
#include <string.h>
//#include <io.h>
//#include <fcntl.h>
#include <posapi.h>

//�����ļ��������󳤶�
#define MAX_LNG_FILE_SIZE  64*1024
//�ַ������������
#define MAX_LNG_STR        2048

typedef struct 
{
	char *src;
	char *dst;
}tStrIdx;

typedef struct 
{
	int begin;
	int count;
}tASCIdx;

static char      fileBuf[MAX_LNG_FILE_SIZE];    //�ļ�����
static tStrIdx   strIdx[MAX_LNG_STR];           //�ַ�������
static tASCIdx   ascIdx[256];    // ascii ����
static int       idxCnt = 0;                  //�������������ַ���������
static int       useExtLng = 0;   // bool ʹ���ⲿ����
static char		 lngName[20];

static int search_compare (const void *x, const void *y);
static int qsort_compare (const void *x, const void *y);

static void *bsearch(const void *key, const void *base, size_t /* nmemb */ high,
			  size_t size, int (*compar)(const void *, const void *));

static void qsort (void  *base,
            size_t nel,
            size_t width,
            int (*comp)(const void *, const void *));
static char *FormatStr(char *str);
static void CreatAscIdx(void);

//����strIdx[i].src[0] ������
static void CreatAscIdx(void)
{
	int i;
	char base;
	int cnt;

	memset(ascIdx,0,sizeof(ascIdx));

	i = 0;
	do {
		cnt = 0;
		base = strIdx[i].src[0];
		ascIdx[(unsigned char)base].begin = i;
		while ((i < idxCnt) && (base == strIdx[i].src[0])) //�������������ɸı�˳��
		{
			i++;
			cnt++;
		}
		ascIdx[(unsigned char)base].count = cnt;
	} while(i < idxCnt);
}


//��ʽ���ַ��У��� "\n"  "\r"  "\t"  "\\"  "\""  "\x32" ת��Ϊ '\n'  '\r'  '\t' '\\' '\"' '\x32'
static char *FormatStr(char *str)
{
				           	//'0','1','2','3','4','5','6','7','8','9',                                              'A','B','C','D','E','F'
const unsigned char tab2[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	int i,j;
	char ch;
	int shift;
	ch = '\0';
	for (i=0; str[i] != '\0'; i++)
	{
		if (str[i] == '\\')
		{
			switch(str[i+1])
			{
				case 'n'  : ch = '\n'; shift = 1; break;
				case 'r'  : ch = '\r'; shift = 1; break;
				case 't'  : ch = '\t'; shift = 1; break;
				case '\\' : ch = '\\'; shift = 1; break;
				case '\"' : ch = '\"'; shift = 1; break;
				case 'x'  : 
					{
						unsigned char c;
						shift = 1;
						c = str[i+2];
						if(c >='a' && c <='z')
							c = c -'a'+'A';
						if(((c>='0' && c<='9') || (c>='A' && c<='F')))
						{
							ch = tab2[c-'0'];
							shift++;
						}
						c = str[i+3];
						if(c >='a' && c <='z')
							c = c -'a'+'A';
						if(((c>='0' && c<='9') || (c>='A' && c<='F')))
						{
							ch <<=4;
							ch |= tab2[c-'0'];
							shift++;
						}	
						
						break;
					}
				default   : ch = '\0'; break;
			}
			
			if (ch != '\0')
			{
				str[i] = ch;
				for (j=i+1; str[j] != '\0'; j++)
				{
					str[j] = str[j+shift];
				}
				str[j] = '\0';
			}
		}
	}
	return str;
}

int SetLng(char *name)
{
	char *pLineBegin=NULL;     // ָ���еĿ�ͷ
	char *pLineEnd=NULL;     // ָ���еĽ�β '\n'
	char *pFile=NULL;           // ���ļ����������ָ��
	int fd;
	int fileSize;

	useExtLng = 0;
	if (name == NULL)
	{
		return -1;
	}

	fd = open(name, O_RDWR);
	if (fd < 0) //���ļ�ʧ��
	{
		return -2;
	}

	if (MAX_LNG_FILE_SIZE <= filesize(name))
	{
		// ��������С�ˣ�Ҫ�Ӵ�MAX_LNG_FILE_SIZE��ֵ��
		return -5;
	}

	fileSize = read(fd,fileBuf,sizeof(fileBuf));
	close(fd);
	if (fileSize <= 0)
	{
		return -3;
	}
	fileBuf[fileSize] = '\0';
	
	// ���ļ����з����������ַ�����������
	idxCnt = 0;
	pFile = fileBuf;
	pLineEnd = NULL;
	pLineBegin = pFile;
	while (pFile <= (fileBuf+fileSize)) // <=  ����ΪҪ�����ļ��Ľ����� '\0'
	{
	    //��Ϊ '\n' ����ĩ ,'\0' ���ļ�����
		if (*pFile == '\n' || *pFile == '\0')
		{
			pLineEnd = pFile;
			*pLineEnd = '\0'; // ����ĩ '\n' ת��Ϊ '\0'
		}
		pFile++;
		if (pLineEnd != NULL )   //�Ѿ��ҵ�һ����
		{
			if (*pLineBegin != '#') // ����ע����
			{
				char *pMark1=NULL; // ָ�� ��һ�� "
				char *pMark2=NULL; // ָ�� �ڶ��� "
				char *pMark3=NULL; // ָ�� ������ "
				char *pMark4=NULL; // ָ�� ���ĸ� "
				char *p;  
				char *pMark2Tmp;
				char *pMark3Tmp;
				int quan=0;
				int quanTmp;

				do{		
					// ȡ��һ�� "
					pMark1 = pLineBegin;
					while(1)
					{
						if (*pMark1 == '\"') 
						{
							// �ҵ���
							break;
						}
						else if (pMark1 >= pLineEnd)
						{
							// �ַ�������
							pMark1 = NULL;
							break;
						}
						pMark1++;
					}
					if (pMark1 == NULL)
					{
						// ��һ��ȡ�����������˳�
						break;
					}

					// ȡ " = ",��ȡ�ڶ�������
					// ��ȡ��Σ����ȡȨ����Ǹ�
					p = pMark1;
					while(p < pLineEnd)
					{
GET_MARK2:
						pMark2Tmp = NULL;
						pMark3Tmp = NULL;
						quanTmp   = 0;
						// �ҵڶ��� "
						while(p < pLineEnd)
						{
							p++;
							if (*p == '\"') 
							{
								pMark2Tmp = p;
								break;
							}
						}
						// ���˿��ַ�,���� =
						while(p < pLineEnd)
						{
							p++;
							if (*p == ' ' || *p == '\t')
							{
								/* do nothing */
							}
							else if (*p == '=')
							{
								// �ҵ� =
								break;
							}
							else 
							{
								goto GET_MARK2;
							}
						}
						if (p >= pLineEnd)
						{
							// �ַ�������
							break;
						}
						// �����Ѿ��ҵ� = ��
						//  ���˿��ַ�,���� ������ "
						while(p < pLineEnd)
						{
							p++;
							if (*p == ' ' || *p == '\t')
							{
								/* do nothing */
							}
							else if (*p == '\"')
							{
								// �ҵ������� "
								pMark3Tmp = p;
								break;
							}
							else 
							{
								goto GET_MARK2;
							}
						}

						if (p >= pLineEnd)
						{
							// �ַ�������
							break;
						}
						p = pMark3Tmp -1; // ����һ��, ������ " ���ܱ�������

						// ���� ĳһ��� �ڶ�������������������
						
						if (*(pMark2Tmp-1) == '\\')
						{
							quanTmp = 1;
						}
						else 
						{
							quanTmp = 3;
						}

						if (quanTmp > quan)
						{
							quan = quanTmp;
							pMark2 = pMark2Tmp;
							pMark3 = pMark3Tmp;
						}
						if (quan == 3)
						{
							break;
						}
					
					}
					if (quan == 0)
					{
						// ȡ�����ڶ����������ţ��˳�
						break;
					}

					// ���� ȡ���� ���� �ڶ�����������


					// ȡ���һ��
					pMark4 = pLineEnd;
					while(1)
					{
						pMark4--;
						// ע�� if (pMark4 <= pMark3) һ��Ҫ���� if (*pMark4 == '\"')  
						if (pMark4 <= pMark3)
						{
							// ���˵������˻��Ҳ���
							pMark4 = NULL;
							break;					
						}
						else if (*pMark4 == '\"') 
						{
							// �ҵ���
							break;
						}

					}
				}while(0);

				if (pMark4 != NULL) // ���в�������ȷ��
				{
					*pMark2 = '\0';   //Դ�ַ����Ľ�����
					*pMark4 = '\0';   //Ŀ���ַ����Ľ�����
					strIdx[idxCnt].src = FormatStr(pMark1 + 1);
					strIdx[idxCnt].dst = FormatStr(pMark3 + 1);
					//printf("\"%s\" -> \"%s\"\n",strIdx[idxCnt].src,strIdx[idxCnt].dst);
					idxCnt++;		
					if (MAX_LNG_STR < idxCnt)
					{
						// MAX_LNG_STR ��С�ˣ�Ҫ�Ӵ�
						return -6;
					}
				}

			} /* end of if (*pLineBegin != '#') */
			pLineBegin = pLineEnd + 1;
			pLineEnd = NULL;
		}
	}
	if (idxCnt == 0)
	{
		return -4;
	}
	else
	{
		qsort(strIdx,idxCnt,sizeof(strIdx[0]),qsort_compare);
	
		CreatAscIdx();
		strncpy(lngName,name,sizeof(lngName));
		useExtLng = 1;
		return 0;
	}

}

//GetLngName
const char *GetLngName(void)
{
	if (useExtLng)
	{
		return lngName;
	}
	else
	{
		return NULL;
	}

}

//translate
char *Translate(char *str)
{
	tStrIdx *line = NULL;
	tASCIdx *idx;

	if (str == NULL)
	{
		return NULL;
	}

	if (!useExtLng) //��ʹ���ⲿ����
	{
		return str;
	}

	//ascIdx[(unsigned char)str[0]] ��¼���� str[0] ��ͷ���ַ���
	//��strIdx�����дӵڼ�����ʼ(begin)���ܹ��м���(count)
	idx = &ascIdx[(unsigned char)str[0]];

	//û�еĻ���ֱ�ӷ��أ�����
	if (idx->count == 0)
	{
		return str;
	}

	//ֻ��һ��ʱ��ֱ�ӷ��أ�����(�������ļ�����ʱ���ܻᷭ�����)
	//if (idx->count == 1)
	//{
	//	return strIdx[idx->begin].dst;
	//}

	//���ֲ���
	line = bsearch(str, &strIdx[idx->begin], 
		idx->count, sizeof(strIdx[0]),search_compare);
	if ((line == NULL) || (line->dst[0] == '\0'))
	{
		return str;
	}
	else
	{
		return line->dst;
	}
	
}

static int search_compare (const void *x, const void *y)
{
  const char *name = x;
  const tStrIdx *line = y;

  return strcmp (name, line->src);
}


//���ֲ����㷨 ���� uClibc
static void *bsearch(const void *key, const void *base, size_t /* nmemb */ high,
			  size_t size, int (*compar)(const void *, const void *))
{
	register char *p;
	size_t low;
	size_t mid;
	int r;

	if (size > 0) {				/* TODO: change this to an assert?? */
		low = 0;
		while (low < high) {
			mid = low + ((high - low) >> 1); /* Avoid possible overflow here. */
			p = ((char *)base) + mid * size; /* Could overflow here... */
			r = (*compar)(key, p); /* but that's an application problem! */
			if (r > 0) {
				low = mid + 1;
			} else if (r < 0) {
				high = mid;
			} else {
				return p;
			}
		}
	}
	return NULL;
}


static int qsort_compare (const void *x, const void *y)
{
  const tStrIdx *line1 = x;
  const tStrIdx *line2 = y;

  return strcmp (line1->src, line2->src);
}


//���������㷨���� uClibc
/* This code is derived from a public domain shell sort routine by
 * Ray Gardner and found in Bob Stout's snippets collection.  The
 * original code is included below in an #if 0/#endif block.
 *
 * I modified it to avoid the possibility of overflow in the wgap
 * calculation, as well as to reduce the generated code size with
 * bcc and gcc. */

static void qsort (void  *base,
            size_t nel,
            size_t width,
            int (*comp)(const void *, const void *))
{
	size_t wgap, i, j, k;
	char tmp;

	if ((nel > 1) && (width > 0)) {
//		assert( nel <= ((size_t)(-1)) / width ); /* check for overflow */
		wgap = 0;
		do {
			wgap = 3 * wgap + 1;
		} while (wgap < (nel-1)/3);
		/* From the above, we know that either wgap == 1 < nel or */
		/* ((wgap-1)/3 < (int) ((nel-1)/3) <= (nel-1)/3 ==> wgap <  nel. */
		wgap *= width;			/* So this can not overflow if wnel doesn't. */
		nel *= width;			/* Convert nel to 'wnel' */
		do {
			i = wgap;
			do {
				j = i;
				do {
					register char *a;
					register char *b;

					j -= wgap;
					a = j + ((char *)base);
					b = a + wgap;
					if ( (*comp)(a, b) <= 0 ) {
						break;
					}
					k = width;
					do {
						tmp = *a;
						*a++ = *b;
						*b++ = tmp;
					} while ( --k );
				} while (j >= wgap);
				i += width;
			} while (i < nel);
			wgap = (wgap - width)/3;
		} while (wgap);
	}
}
