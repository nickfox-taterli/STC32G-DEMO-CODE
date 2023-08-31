#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "normal_complex_caculate.h"
#include "middle.h"
#include "keyboard_input.h"
#include "keyboard_device.h"
#include "alg_caculate.h"
#include "alg_linearlist.h"
//#include "eeprom_rw.h"

#define HISTORY_SIZE 60

uchar code history_add[] = {0,26,52,78,104,130,156,182,208};		//��ʷ��¼�ص���ַ

uchar g_chCalcStatus;
double g_ans;

void push_history(char *pExpression);
void pop_history(char *pExpression, char loc);


void normal_calculate(void)
{
	uchar i,flag;
	char input_buf[26];
	char output_buf[16];
	uchar chInputStatus = INPUT_VACANT;
	
	char top,bottom,current;	//������ʷ��¼���
	double re_result;
		
	uchar num1 = 0xff;

	top = 0;
	bottom = 0;
	
	lcd_clear();
	g_chCalcStatus = CALC_NORMAL;
	
	while(1)
	{
		if(INPUT_VACANT == chInputStatus)		//��������Ҫ����
		{
			for(i=0;i<25;i++)
				input_buf[i]=' ';		//��մ�
			chInputStatus = input_to_str(0,input_buf,num1);
		}		
		
		if(INPUT_EQ == chInputStatus)
		{
			chInputStatus = INPUT_VACANT;	//����
			if(LArray_TRUE == alg_calc_expression(input_buf, &re_result, NULL))
			{
				sprintf(output_buf,"%g",re_result);
				g_ans = re_result;					//����ans
				lcd_set_pos(3,0);
				lcd_write_str("                ");
				lcd_set_pos(3,0);
				lcd_write_str(output_buf);
				push_history(input_buf);			//ѹ����ʷ��¼
				current = byte_read(ADD_TOP_HISTORY, 0);	//���µ�ǰλ��
			}
			else 
			{
				sprintf(output_buf,"INPUT ERROR");
				lcd_set_pos(3,0);
				lcd_write_str("                ");
				lcd_set_pos(3,0);
				lcd_write_str(output_buf);
			}
			
		}

		else if(INPUT_UP == chInputStatus || INPUT_DOWN == chInputStatus)	//���·�ҳ
		{
			flag = 0xff;
			if(INPUT_UP == chInputStatus)		//�Ϸ�ҳ
			{
				bottom = byte_read(ADD_BOTTOM_HISTORY, 0);		//��ȡ��ǰ�����͵ײ����
				if(current-1!=bottom || (current == 0 && bottom != HISTORY_SIZE-1))					//û�з���		����current-1
				{	
					if(current == 0)
						current = HISTORY_SIZE-1;
					else
						current -= 1;
					
					flag = 1;
				}
				else 
					chInputStatus = input_to_str(0,input_buf,0xff);		//д���ʽ
			}
			
			else if(INPUT_DOWN == chInputStatus)
			{
				top = byte_read(ADD_TOP_HISTORY, 0);
				if(current!=top)					//û�з�����
				{	
					if(current == HISTORY_SIZE-1)
						current = 0;
					else
						current += 1;
				}
				flag = 1;
			}
			
			if(flag == 1)
			{
				flag = 0xff;
				for(i=0;i<26;i++)
					input_buf[i]=' ';		//��մ�
				pop_history(input_buf, current);	//��ȡֵ
				lcd_clear();						//����
				if(LArray_TRUE == alg_calc_expression(input_buf, &re_result, NULL))		//��ʾ���
				{
					sprintf(output_buf,"%g",re_result);
					lcd_set_pos(3,0);
					lcd_write_str("                ");
					lcd_set_pos(3,0);
					lcd_write_str(output_buf);
				}
				
				chInputStatus = input_to_str(0,input_buf,0xff);		//д���ʽ
			}
		}

		
				
		if(INPUT_AC == chInputStatus)
		{
			lcd_clear();
			num1 = 0xff;
			chInputStatus = INPUT_VACANT;	//ָ������
			current = byte_read(ADD_TOP_HISTORY, 0);	//��ʷ��λ
			if(current == HISTORY_SIZE-1)
				current = 0;
			else
				current += 1;
			continue;
		}
		
		if(INPUT_VACANT == chInputStatus)
		{
			num1=keyboard_output();
			lcd_clear();				//����
		}
	}
}

void complex_calculate(void)
{
	uchar i;
	char input_buf[26];
	char output_buf[16];
	uchar chInputStatus;
	double re_result,im_result;
	uchar num1 = 0xff;
	lcd_clear();
	g_chCalcStatus = CALC_COMPLEX;
	
	while(1)
	{
		for(i=0;i<25;i++)
			input_buf[i]=' ';		//��մ�
		do
		{
			chInputStatus = input_to_str(0, input_buf,num1);			//���봮
		}while(INPUT_EQ != chInputStatus && INPUT_AC != chInputStatus);
		
		if(INPUT_EQ == chInputStatus)
		{
			if(LArray_TRUE == alg_calc_expression(input_buf, &re_result, &im_result))
			{
				if(im_result <0.0)
					sprintf(output_buf,"%g%gi",re_result,im_result);
				else
					sprintf(output_buf,"%g+%gi",re_result,im_result);
			}
			else 
				sprintf(output_buf,"INPUT ERROR");
			lcd_set_pos(3,0);
			lcd_write_str("                ");
			lcd_set_pos(3,0);
			lcd_write_str(output_buf);
		}
		
		else if(INPUT_AC == chInputStatus)
		{
			lcd_clear();
			num1 = 0xff;
			continue;
		}
		
		num1=keyboard_output();
		lcd_clear();

	}
}

void push_history(char *pExpression)
{
	char top, bottom;
	char i;
	
	top = byte_read(ADD_TOP_HISTORY, 0);
	bottom = byte_read(ADD_BOTTOM_HISTORY, 0);		//��ȡ��ǰ�����͵ײ����

	if(bottom == 0 && top == HISTORY_SIZE-1)	//��ǰ�Ѿ���д��
	{
		bottom += 1;
		top = 0;
	}
	else if(bottom - top == 1)					//��ǰ�Ѿ���д��							
	{
		if(bottom == HISTORY_SIZE-1)
			bottom = 0;
		else 
			bottom += 1;
		if(top == HISTORY_SIZE-1)
			top = 0;
		else
			top += 1;
	}
	else
	{
		top += 1;
	}
	
	for(i=0;i<26;i++)
	{
		byte_write(history_add[top%9]+i, pExpression[i], top/9+1);		//д����ʽ
		if(pExpression[i] == '\0')
			break;
	}
	
	byte_write(ADD_TOP_HISTORY, top, 0);
	byte_write(ADD_BOTTOM_HISTORY, bottom, 0);		//д��
}

void pop_history(char *pExpression,char loc)
{
	char i;
		
	for(i=0;i<26;i++)
	{
		pExpression[i] = byte_read(history_add[loc%9]+i, loc/9+1);		//д����ʽ
		if(pExpression[i] == '\0')
			break;
	}

}