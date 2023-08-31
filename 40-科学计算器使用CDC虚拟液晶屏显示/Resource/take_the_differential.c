#include <stdio.h>
#include <math.h> 
#include "take_the_differential.h"
#include "middle.h"
#include "alg_atof.h"
#include "keyboard_device.h"
#include "keyboard_input.h"
#include "alg_caculate.h"

#define PRECISION 0.01
extern uchar num0;
extern double x;
extern uchar g_chCalcStatus;
void take_the_derivative( uchar *hanshu_buff, double x0 , uchar *result_output) ;//���� ����    һ����ָ�� ( �������ؽ�� ) ;

//�󵼺��� 
void run_the_differential()
{
	uchar hanshu_buff[26];//�����溯�����ʽ 
	uchar x0_buff[26];//�����溯�����ʽ 
	uchar result_output[17];//��Ž��
	double x0=0;
	uchar judge=0xff;
	//��ʼ�� 
	uchar i=0;
	for(i=0;i<26;i++)
	{
		hanshu_buff[i]=x0_buff[i]=' ';
	}
	for(i=0;i<17;i++)
	{
		result_output[i]=' ';
	}
	//������Ҫ����
	g_chCalcStatus = CALC_NORMAL; 
	num0=0xff;//��ʼ��
	while(1) 
	{
		judge=0xff;
		//��ʼ���˵���ʾ
		lcd_clear();
		lcd_clear_pic();
		lcd_set_pos(0,0);
		lcd_write_str("1 ���뺯��"); 
		lcd_set_pos(1,0);
		lcd_write_str("2 ����x0"); 
		lcd_set_pos(2,0);
		lcd_write_str("3 �󵼽��"); 
//		
		num0=0xff;
		do
		{
			num0=keyboard_output();//��ȡ����ֵ, ֱ������1,2,3
		}while(num0!='1' && num0!='2' && num0!='3' );
		
		
		lcd_clear();
		lcd_clear_pic();
		
		switch(num0)
		{
			case '1':
				lcd_set_pos(0,0);
				lcd_write_str("y =");
				lcd_set_pos(1,0);
				for(i=0; i<16 && hanshu_buff[i]!=0 ; i++ )
				{
					lcd_write_dat( hanshu_buff[i] ) ;
					if(i==15)
					{
						lcd_set_pos(2,0);
						for(i=16; i<25 && hanshu_buff[i]!=0 ; i++ )
						{
							lcd_write_dat( hanshu_buff[i] ) ;
						}
					}	
				} 
				//��ȡ����
			
						//�ڵ�3�����뺯�� 
				do
				{
					for(i=0;i<26;i++)
					{
						hanshu_buff[i]=0;
					} 
					judge = input_to_str(3,hanshu_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				
				break;
				
				
			case '2': 
				lcd_set_pos(0,0);
				lcd_write_str("x0 =");
				lcd_set_pos(1,0);
				sprintf( x0_buff,"%g",x0 ); 
				lcd_write_str(x0_buff);
				//��ȡx0 
//look			
				for(i=0;i<26;i++)
				{
					x0_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,x0_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					x0 = alg_atof(x0_buff);	
				}
				break;
				
			case '3':
				lcd_set_pos(0,0);
				lcd_write_str("x0 =");
				lcd_set_pos(1,0);
				sprintf( x0_buff,"%g",x0 ); 
				lcd_write_str(x0_buff);
				lcd_set_pos(2,0);
				lcd_write_str("y' =");
				//�����󵼳��� 
				take_the_derivative( hanshu_buff, x0, result_output );
				while(keyboard_output()!=19);
				
				break;
		} 	
	}
	
	 
	
}


	
/*************���Ӻ���*********************
//���룺double x0 ����ź������ַ���							    	*/ 	

/*********����˼· ******** 
//���躯���� y = 1/x ��

//�ж�

//1 �Ƿ��ڶ������� 

//2 ���ɵ���
//	a �жϵ�ʱ�򣬼���dx֮�������û�����ֵ ����>����Ҫ�ٵ����жϺ������жϼ���dx֮���ǲ����ڶ�������
//	b ��������->������	//�����������Ĳ�ֵ ���� ���ǵĺͣ��ж�һ�²�ֵռ�ȴ�С��̫����ǲ��ɵ� 

//3 ��������ȷ�� 
// �������ޣ�ֱ������߾��ȵ�dxȥ��ͺ��ˣ�û�취��� 

	//interesting ,/0 bug,/0.0��ͨ��			*/
	 
void take_the_derivative( uchar *hanshu_buff, double x0 , uchar *result_output) //���� ����    һ����ָ�� ( �������ؽ�� ) 
{
    
	double dx= 0.000001; 
	
	double positive_derivative = 0.0;
	double negative_derivative = 0.0;
	
	double differential_result = 0.0;
	
	uchar flag_wrong  = 0;
	uchar i=0;
	double temp_0=0;
	double temp_1=0;
	

//	uchar result_output[17];//������Ž�� 
	for(i=0;i<16;i++)
	{
		result_output[i]=' ';
	}
	result_output[16]=0;
	
//    alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);
		x = x0; 
		if( alg_calc_expression( hanshu_buff , 0 , 0 )==1 )
		{
			x = x0 + dx;
			if( alg_calc_expression( hanshu_buff , 0 , 0 )==1 )
			{
				x = x0 - dx;
				if( alg_calc_expression( hanshu_buff , 0 , 0 )==1 )
					;
				else
					flag_wrong=1;
			}
			else
				flag_wrong=1;
		}
		else 
			flag_wrong=1;
			
		if(flag_wrong==0)
		{
			x = x0;
			alg_calc_expression( hanshu_buff , &temp_1  , 0 );
			
			x = x0 + dx;
			alg_calc_expression( hanshu_buff , &temp_0  , 0 );
			
			positive_derivative = ( temp_0 - temp_1  )/dx; 
			
			x = x0 - dx;
			alg_calc_expression( hanshu_buff , &temp_0  , 0 );
			
			negative_derivative = ( temp_0 - temp_1  )/(-dx);
		
			
//			if(  fabs(   ( positive_derivative - negative_derivative )
//		    	 	/( positive_derivative + negative_derivative ) ) < PRECISION )
//								//����ľ��������ж��Ƿ������������������󣬼��ж��Ƿ�õ�ɵ� 
//			{
				differential_result = ( positive_derivative + negative_derivative ) /2;
				
				sprintf(result_output, "%g",differential_result);
				lcd_set_pos(3,0);
				lcd_write_str( result_output);
//			}
//			else
//			{
//				//��ʾ���� 	
//				lcd_set_pos(3,0);
//				lcd_write_str("���ɵ�");	
//			}
		} 
//		else if(flag_wrong==1)	//��ʾ����
//		{
//			lcd_set_pos(3,0);
//			lcd_write_str( "���벻�Ϸ�" );
//		}
		
	
}


///*************�ж��Ƿ��ڶ�������*************
//���룺f(x)�������� double f
//���أ�TRUE 
//	  FALSE							    	*/ 
//	  
//int judge_domain_of_definition( double f ) 
//{
//	char str[8];
//	char str_error[]="1.#INF";	//����ֵ 
//	char str_error0[]="-1.#INF";
//	gcvt( f, 7,str);			//����gcvt��������fת�����ַ����������ֵ�Ա�
//	if(strcmp(str,str_error)==0 ||strcmp(str,str_error0)==0 )//�ж�һ���Ƿ��ڶ������ڣ�������ڶ������ڣ��᷵��0��������ʾ "�������x���ڴ˺����Ķ�������"  
//	{
//		printf("�������x���ڴ˺����Ķ�������\n");
//		return FALSE;
//	}
//	else
//		return TRUE;	
//} 

