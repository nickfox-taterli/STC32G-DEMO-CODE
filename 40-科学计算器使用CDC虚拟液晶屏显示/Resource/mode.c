#include <stdlib.h>
#include <stdio.h>
//#include "STC12C5A60S2.h"
#include "mode.h"
#include "keyboard_input.h"
#include "keyboard_device.h"
#include "alg_caculate.h"
#include "alg_linearlist.h"
//#include "lcd_12864.h"
//#include "eeprom_rw.h"
#include "normal_complex_caculate.h"
#include "function_pic.h"
#include "matrix.h"
#include "solve_equation.h"
#include "take_the_differential.h"
#include "statistics.h"
//#include "pwm.h"
#include "solve_differential_equation.h"
//#include "time.h"
//#include "change_units.h"
#include "middle.h"

#define CASE break;case

uint time_out = 0;		//�ڵ糬ʱ

void mode_real_calculate(void);
void mode_complex_calculate(void);
void mode_change(void);
void mode_set(void);
void mode_sleep(void);

uchar mode_choose(void)
{
	switch(keyboard_output())
	{
		case '0': return 0;	//ѡ��0

		CASE '1': return 1;	//ѡ��1

		CASE '2': return 2;	//ѡ��2

		CASE '3': return 3;	//ѡ��3

		CASE '4': return 4;	//ѡ��4

		CASE '5': return 5;	//ѡ��5

		CASE '6': return 6;	//ѡ��6

		CASE '7': return 7;	//ѡ��7

		CASE '8': return 8;	//ѡ��8

		CASE '9': return 9;	//ѡ��9
				
		CASE 3 : return 10;	//��
		
		CASE 6 : return 11; //��
		
	}
	return 13;			//�������
}

void mode_select(void)
{
	uchar chProgramStatus;
	close_cursor();		//�ع��

	chProgramStatus = byte_read(ADD_PROG_STATUS, 0);
	time_out = byte_read(ADD_TIMEOUT, 0);					//��ȡ��ʱ�趨
	
	switch(chProgramStatus)
	{
		case NORMAL 		: normal_calculate();
		case COMPLEX		: complex_calculate();
		case MATRIX		 	: ju_main();
		case DRAW_PIC		: draw_pic();
		case SOLVE_EQUATION : solve_equation();
	//	case TAKE_DIFFER	: run_the_differential();
		case STASTISTICS	: sta_menu1();
		case MODE			: mode_change();
		case SET			: mode_set();
		case DIFFER_EQUATION: solve_differential_equation();
	//	case CHANGE_UNITS   : change_units();

	}
}

void mode_change(void)
{
	uchar m;
	uchar count1 = 1;
	uchar out = 0;
	uchar chStatus;
	
	while(1)
	{
		close_cursor();
		lcd_clear();

		if(count1>4)
			count1=4;
		else if(count1<1)
			count1=1;
		for(m=0;m<4;m++)
			{
				lcd_set_pos(m,0);	
				switch (m+count1)
					{
						case 1:
							lcd_write_str("1.��ͨ����");break;
						case 2:
							lcd_write_str("2.��������");break;
						case 3:
							lcd_write_str("3.��������");break;
						case 4:
							lcd_write_str("4.����ͼ��");break;
						case 5:
							lcd_write_str("5.�ⷽ��");break;
						case 6:
							lcd_write_str("6.ͳ��");break;
						case 7:
							lcd_write_str("7.һ�׳�΢�ַ���");break;
//						case 8:
//							lcd_write_str("8.һ�׳�΢�ַ���");break;
				//		case 9:
				//			lcd_write_str("9.��λת��");break;
						}
				}	

		switch(mode_choose())
		{
			case 1 : chStatus = NORMAL;				out = 1;	break;
			case 2 : chStatus = COMPLEX;			out = 1;	break;
			case 3 : chStatus = MATRIX;				out = 1;	break;
			case 4 : chStatus = DRAW_PIC;			out = 1;	break;
			case 5 : chStatus = SOLVE_EQUATION;		out = 1;	break;
			case 6 : chStatus = STASTISTICS;		out = 1;	break;
			case 7 : chStatus = DIFFER_EQUATION;	out = 1;	break;
			case 10 : count1--;break;
			case 11 : count1++;break;
		}
		if(out == 1)
			break;
	}
	
	byte_write(ADD_PROG_STATUS, chStatus,0);		//д�뵱ǰָ��
	bl_bright_change(byte_read(ADD_BRIGHT, 0));			//��ȡ����
	
	reset();		//��λ
}

void mode_set(void)
{
	uchar m;
	uchar count1 = 1;
	uchar out = 0;
	
	uchar set_value;
	
	while(1)
	{
		close_cursor();
		lcd_clear();

		if(count1>3)
			count1=3;
		else if(count1<1)
			count1=1;
		for(m=0;m<4;m++)
			{
				lcd_set_pos(m,0);	
				switch (m+count1)
					{
						case 1:
							lcd_write_str("1.��Ļ����");break;
						case 2:
							lcd_write_str("2.shift ��ʾ");break;
						case 3:
							lcd_write_str("3.�ڵ�ѡ��");break;
						case 4:
							lcd_write_str("4.��������");break;
						case 5:
							lcd_write_str("5.ʱ������");break;
						case 6:
							lcd_write_str("6.����");break;
//						case 7:
//							lcd_write_str("7.����");break;
						}
				}	
		
		
		switch(mode_choose())
		{
			case 10 : count1--;break;
			case 11 : count1++;break;				//�ƶ���Ļ
			
			case 1 : 
					
					while(1)
					{
						lcd_clear();
						lcd_inverse(1, 5, 1);							//����
						lcd_set_pos(1,0);
						lcd_write_str("��Ļ����");
						
						set_value = byte_read(ADD_BRIGHT, 0);			//������
						bl_bright_change(set_value);
						
						lcd_set_pos(1,5);
						lcd_write_dat((uchar)(set_value+'0'));
						
						if(set_value > 1)
						{
							lcd_set_pos(2,5);
							lcd_write_dat(0x19);					//�¼�ͷ
						}
						if(set_value < 5)
						{
							lcd_set_pos(0,5);
							lcd_write_dat(0x18);					//�ϼ�ͷ
						}
						switch(keyboard())
						{
							case 4	:	if(set_value < 5)//��
											set_value ++;
										break;
										
							case 9	:	if(set_value>1)//��
											set_value --;
										break;
							case 25	:			//AC
							case 40	:	out = 1;//=
										break;
						}
						byte_write(ADD_BRIGHT, set_value, 0);			//д����
						if(out == 1)
						{
							out = 0;
							break;
						}
							
					}

					lcd_clear_pic();								//�巴��
					break;

			case 2 : 
					while(1)
					{
						lcd_clear();
						lcd_inverse(1, 6, 1);							//����
						lcd_set_pos(1,0);
						lcd_write_str(" shift��ʾ");
						lcd_set_pos(3,1);
						lcd_write_str("ͨ���ı�����");
						
						set_value = byte_read(ADD_SHIFTSIGN, 0);			//������
						bl_bright_change(set_value);
						
						lcd_set_pos(1,6);
						lcd_write_dat((uchar)(set_value+'0'));
						
						if(set_value > 1)
						{
							lcd_set_pos(2,6);
							lcd_write_dat(0x19);					//�¼�ͷ
						}
						if(set_value < 5)
						{
							lcd_set_pos(0,6);
							lcd_write_dat(0x18);					//�ϼ�ͷ
						}
						switch(keyboard())
						{
							case 4	:	if(set_value < 5)//��
											set_value ++;
										break;
										
							case 9	:	if(set_value>1)//��
											set_value --;
										break;
							case 25	:			//AC
							case 40	:	out = 1;//=
						}
						
						byte_write(ADD_SHIFTSIGN, set_value, 0);			//д����
						
						if(out == 1)
						{
							out = 0;
							break;
						}
							
					}
					bl_bright_change(byte_read(ADD_BRIGHT, 0));			//��ԭԭ������
					lcd_clear_pic();								//�巴��
					break;
					
			case 3 : 
					while(1)
					{
						lcd_clear();
						lcd_inverse(2, 4, 1);							//����
						lcd_set_pos(0,0);
						lcd_write_str("�ڵ�ģʽ");
						lcd_set_pos(2,1);
						lcd_write_str("�޲���");
						lcd_set_pos(2,5);
						lcd_write_str("���Ӻ�");
						
						set_value = byte_read(ADD_TIMEOUT, 0);			//��ʱ��
						
						lcd_set_pos(2,4);
						lcd_write_dat((uchar)(set_value+'0'));
						
						if(set_value > 1)
						{
							lcd_set_pos(3,4);
							lcd_write_dat(0x19);					//�¼�ͷ
						}
						if(set_value < 9)
						{
							lcd_set_pos(1,4);
							lcd_write_dat(0x18);					//�ϼ�ͷ
						}
						switch(keyboard())
						{
							case 4	:	if(set_value < 9)//��
											set_value ++;
										break;
										
							case 9	:	if(set_value>1)//��
											set_value --;
										break;
							case 25	:			//AC
							case 40	:	out = 1;//=
										break;
						}
						
						byte_write(ADD_TIMEOUT, set_value, 0);			//дʱ��
						if(out == 1)
						{
							out = 0;
							break;
						}
							
					}
					lcd_clear_pic();								//�巴��
					break;
			case 4 : general_dateset();			out = 1;	break;
			case 5 : general_timeset();			out = 1;	break;
			
			case 6 : about();					out = 1;	break;
//			case 7 : chProgramStatus = STASTISTICS;		out = 1;	break;
//			case 10 : count1--;break;
//			case 11 : count1++;break;
		}
	}
}

void mode_sleep(void)	//�͹���ģʽ
{
	close_cursor();
	lcd_clear();
	bl_bright_change(1); //��������
	page_time();
}


void reset(void)
{
	IAP_CONTR = 0x20;			//��0x20����λ��������
}