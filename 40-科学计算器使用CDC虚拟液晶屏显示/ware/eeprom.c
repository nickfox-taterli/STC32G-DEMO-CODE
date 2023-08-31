#include "eeprom.h"


void IapIdle()
{
    IAP_CONTR = 0; //�ر� IAP ����
    IAP_CMD = 0; //�������Ĵ���
    IAP_TRIG = 0; //��������Ĵ���
    IAP_ADDRE = 0x00;
    IAP_ADDRH = 0x00;
    IAP_ADDRL = 0x00; 
}
char IapRead(unsigned long addr)
{
    char dat;
    IAP_CONTR = 0x80; //ʹ�� IAP
    IAP_TPS = 12; //���õȴ����� 12MHz
    IAP_CMD = 1; //���� IAP ������
    IAP_ADDRL = addr; //���� IAP �͵�ַ
    IAP_ADDRH = addr >> 8; //���� IAP �ߵ�ַ
    IAP_ADDRE = addr >> 16; //���� IAP ��ߵ�ַ
    IAP_TRIG = 0x5a; //д��������(0x5a)
    IAP_TRIG = 0xa5; //д��������(0xa5)
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    dat = IAP_DATA; //�� IAP ����
    IapIdle(); //�ر� IAP ����
    return dat;
}
void IapProgram(unsigned long addr, char dat)
{
    IAP_CONTR = 0x80; //ʹ�� IAP
    IAP_TPS = 12; //���õȴ����� 12MHz
    IAP_CMD = 2; //���� IAP д����
    IAP_ADDRL = addr; //���� IAP �͵�ַ
    IAP_ADDRH = addr >> 8; //���� IAP �ߵ�ַ
    IAP_ADDRE = addr >> 16; //���� IAP ��ߵ�ַ
    IAP_DATA = dat; //д IAP ����
    IAP_TRIG = 0x5a; //д��������(0x5a)
    IAP_TRIG = 0xa5; //д��������(0xa5)
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    IapIdle(); //�ر� IAP ����
}
void IapErase(unsigned long addr)
{
    IAP_CONTR = 0x80; //ʹ�� IAP
    IAP_TPS = 12; //���õȴ����� 12MHz
    IAP_CMD = 3; //���� IAP ��������
    IAP_ADDRL = addr; //���� IAP �͵�ַ
    IAP_ADDRH = addr >> 8; //���� IAP �ߵ�ַ
    IAP_ADDRE = addr >> 16; //���� IAP ��ߵ�ַ
    IAP_TRIG = 0x5a; //д��������(0x5a)
    IAP_TRIG = 0xa5; //д��������(0xa5)
    _nop_();
    _nop_();
    _nop_();
    _nop_(); //
    IapIdle(); //�ر� IAP ����
}


//IapErase(0x0400);
//P0 = IapRead(0x0400); //P0=0xff
//IapProgram(0x0400, 0x12);
//P1 = IapRead(0x0400); //P1=0x12
