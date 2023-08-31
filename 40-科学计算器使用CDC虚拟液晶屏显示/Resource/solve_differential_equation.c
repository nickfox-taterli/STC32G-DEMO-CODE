#include <stdio.h>
#include <math.h> 
#include "solve_differential_equation.h"
#include "middle.h"
#include "alg_atof.h"
#include "keyboard_device.h"
#include "keyboard_input.h"
#include "alg_caculate.h"

extern uchar num0;
extern double x,y;
extern uchar g_chCalcStatus;


//��΢�ַ��̺��� 
void solve_differential_equation()
{
	uchar hanshu_buff[26];//�����溯�����ʽ 
	uchar temp_buff[26];//������һЩʽ�� 
	uchar result_output[17];
	double a_shuzhi=0;
	double b_shuzhi=0; 
	double h_shuzhi=0;
	double y_shuzhi=0;
	double x_array[10];//΢�ַ��̵Ľ���б� 
	double y_array[10];
	int n;//����� 
	double temp_0  =0;//��ʱֵ 
	double temp_1  =0;
	uchar judge=0xff;//�ж�ֵ 
	
	uchar m;			//���ڲ˵��������� 
	uchar count1 = 1;
	
	//��ʼ�� 
	uchar i=0;
	for(i=0;i<26;i++)
	{
		hanshu_buff[i]=temp_buff[i]=' ';
	}
	for(i=0;i<16;i++)
		result_output[i]=' ';
	result_output[16]=0;
	
	//������Ҫ����
	g_chCalcStatus = CALC_NORMAL; 
	num0=0xff;//��ʼ��
	
	while(1) 
	{
		judge=0xff;
		//��ʼ���˵���ʾ
		lcd_clear();
		lcd_clear_pic();
		
		if(count1>3)
			count1=3;
		else if(count1<1)
			count1=1;
		for(m=0;m<4;m++)
		{
			lcd_set_pos(m,0);	
			switch ('0'+m+count1)
			{
				case '1':
					lcd_write_str("1.�ұߵĺ�����");break;
				case '2':
					lcd_write_str("2.��˵�a");break;
				case '3':
					lcd_write_str("3.�Ҷ˵�b");break;
				case '4':
					lcd_write_str("4.����h");break;
				case '5':
					lcd_write_str("5.��˵��y ֵ");break;
				case '6':
					lcd_write_str("6.���");break;
				}
		}
		//����ѡ��˵�ѡ�� 
		num0=0xff;
		do
		{
			num0=keyboard_output();//��ȡ����ֵ, ֱ������1,2,3,4,5,6���ߡ� �� 
		}while(num0!='1' && num0!='2' && num0!='3'&& num0!='4' && num0!='5' && num0!='6' && num0!=3 &&num0!=6);
		
		//���ݰ���ѡ��ͬ�Ĺ��� 
		lcd_clear();
		lcd_clear_pic();
		
		switch(num0)
		{
			case 3:
				count1--;//�˵���ʾ���� 
				break;
			case 6:
				count1++;//�˵���ʾ���� 
				break;
			case '1':
				lcd_set_pos(0,0);
				lcd_write_str("�ұߵĺ�����");
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
				lcd_write_str("a =");
				lcd_set_pos(1,0);
				sprintf( temp_buff,"%g",a_shuzhi ); 
				lcd_write_str(temp_buff);
				//��ȡa
//look			
				for(i=0;i<26;i++)
				{
					temp_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,temp_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					a_shuzhi = alg_atof(temp_buff);	
				}
				break;
				
			case '3': 
				lcd_set_pos(0,0);
				lcd_write_str("b =");
				lcd_set_pos(1,0);
				sprintf( temp_buff,"%g",b_shuzhi ); 
				lcd_write_str(temp_buff);
				//��ȡb
//look			
				for(i=0;i<26;i++)
				{
					temp_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,temp_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					b_shuzhi = alg_atof(temp_buff);	
				}
				break;
			case '4': 
				lcd_set_pos(0,0);
				lcd_write_str("h =");
				lcd_set_pos(1,0);
				sprintf( temp_buff,"%g",h_shuzhi ); 
				lcd_write_str(temp_buff);
				//��ȡh
//look			
				for(i=0;i<26;i++)
				{
					temp_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,temp_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					h_shuzhi = alg_atof(temp_buff);	
				}
				break;
			
			case '5': 
				lcd_set_pos(0,0);
				lcd_write_str("��˵��y ֵ =");
				lcd_set_pos(1,0);
				sprintf( temp_buff,"%g",y_shuzhi ); 
				lcd_write_str(temp_buff);
				//��ȡy
//look			
				for(i=0;i<26;i++)
				{
					temp_buff[i]=' '; 
				}
				do
				{
					judge = input_to_str(3,temp_buff,0xff);
				}while(judge !=INPUT_EQ && judge !=INPUT_AC);
				if( judge ==INPUT_EQ )
				{
					y_shuzhi = alg_atof(temp_buff);	
				}
				break;	
				
			case '6'://��� 
				n=(int)(b_shuzhi-a_shuzhi)/h_shuzhi;
				if(a_shuzhi>b_shuzhi)
				{
					lcd_set_pos(0,0);
					lcd_write_str("a>b,wrong");
				} 
				else if(h_shuzhi<0)
				{
					lcd_set_pos(0,0);
					lcd_write_str("h<0,wrong");
				}	
				else if(n>10)
				{
					lcd_set_pos(0,0);
					lcd_write_str("�볢����С(b-a)");
					lcd_set_pos(1,0);
					lcd_write_str("��������h"); 
				}	
				else
				{
					x_array[0]=a_shuzhi;
					y_array[0]=y_shuzhi;
					for(i=0;i<n+1;i++)
				    {
					
				        x_array[i+1]=x_array[i]+h_shuzhi;
				        
				    //  uchar alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);
					//  y[i+1]=y[i]+h * g(x[i],y[i]);
						x=x_array[i];
				        y=y_array[i];
				        alg_calc_expression(hanshu_buff, &temp_0, 0);
				        y_array[i+1]=y_array[i] + h_shuzhi * temp_0;
				    //  y[i+1]=y[i]+h/2*( g( x[i],y[i] ) + g( x[i+1],y[i+1] ) );
				    	x=x_array[i+1];
				        y=y_array[i+1];
				        alg_calc_expression(hanshu_buff, &temp_1, 0);
				    	y_array[i+1]=y_array[i]+h_shuzhi/2*( temp_0+ temp_1 );
				    }
				    i=0;
				    do
				    {
				    	lcd_clear();
						lcd_set_pos(0,0);//x[i]= 
						lcd_write_str("x[");
						if(i/10!=0)
							lcd_write_dat(i/10+'0');
						lcd_write_dat(i%10+'0');
						lcd_write_str("]=");
						lcd_set_pos(0,7);//��ʾ��ͷ��ʾ 
						if(i==0)
						{
							lcd_write_dat(' ');
							lcd_write_dat(0x1a);
						}
						else if(i==n)
						{
							lcd_write_dat(0x1b);
							lcd_write_dat(' ');
						}
						else
						{
							lcd_write_dat(0x1b);
							lcd_write_dat(0x1a);
						}
						
						lcd_set_pos(1,0);
						sprintf(result_output,"%g",x_array[i]);
						lcd_write_str(result_output);
						
						
						lcd_set_pos(2,0);//y[i]= 
						lcd_write_str("y[");
						if(i/10!=0)
							lcd_write_dat(i/10+'0');
						lcd_write_dat(i%10+'0');
						lcd_write_str("]=");
						sprintf(result_output,"%g",y_array[i]);
						lcd_set_pos(3,0);
						lcd_write_str(result_output);
						
						num0=keyboard_output();
						if(num0==5)
						{
							if(i>0)
								i--;
						}
						else if(num0==7)
						{
							if(i<n)//С�� //-1+1
								i++;
						}
							
					}while(num0!=19);//�������AC�� 
				}
				break;
		} 	
	}
	
}


