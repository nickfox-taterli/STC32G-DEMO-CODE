#ifndef __EEPROM_H
#define __EEPROM_H

#include "stc32g.h" //ͷ�ļ����������
#include "intrins.h"

void IapIdle();
char IapRead(unsigned long addr); 
void IapProgram(unsigned long addr, char dat);
void IapErase(unsigned long addr);


#endif

