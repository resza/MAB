#ifndef _LNG_H_
#define _LNG_H_

// _T ���÷��뺯�������ڱ�������Translate�����Բ�������ȫ�ֱ����ĳ���
#define _T(str) Translate(str)

// _T_NOOP ֻ��һ����ʶ��Ϊ��pl�����ܹ�������һ������ȫ�ֱ����ı�ʶ���������ַ����ı�ʶ��
#define _T_NOOP(str) str

int SetLng(char *name); //�������ԣ�nameΪ�����ļ������֣�����1���óɹ�������0������������
const char *GetLngName(void); //����NULL��ʾʹ���ڲ����ԣ������ַ���Ϊ��ǰʹ�����Ե�����
char *Translate(char *str); //���뺯����ͨ���� _T ��ʹ��


#endif
