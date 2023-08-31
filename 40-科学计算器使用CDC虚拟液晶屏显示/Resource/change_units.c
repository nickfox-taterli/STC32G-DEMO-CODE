#include <stdio.h>
#include <math.h> 
#include "change_units.h"
#include "alg_atof.h"
#include "keyboard_device.h"
#include "keyboard_input.h"
#include "alg_caculate.h"
#include "middle.h"

extern uchar num0;
extern double x;
extern uchar g_chCalcStatus;


//��λת�� 
void change_units()
{
	uchar temp_buff[26];//������һЩʽ�� 
	uchar result_output[17];
	double a_shuzhi=0;
	double b_shuzhi=0;
	
	uchar judge=0xff;//�ж�ֵ 
	
	uchar m;			//���ڲ˵��������� 
	uchar count1 = 1;
	
	//��ʼ�� 
	uchar i=0;
	for(i=0;i<26;i++)
	{
		temp_buff[i]=' ';
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
					lcd_write_str("1.����");break;
				case '2':
					lcd_write_str("2.�¶�");break;
				case '3':
					lcd_write_str("3.����");break;
				case '4':
					lcd_write_str("4.�ٶ�");break;
				case '5':
					lcd_write_str("5.�Ƕ�");break;
				}
		}
		//����ѡ��˵�ѡ�� 
		num0=0xff;
		do
		{
			num0=keyboard_output();//��ȡ����ֵ, ֱ������1,2,3,4,5,6���ߡ� �� 
		}while(num0!='1' && num0!='2' && num0!='3'&& num0!='4' && num0!='5' && num0!=3 &&num0!=6);
		
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
			case '1'://1Ӣ��=1.54cm 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.����-");
						lcd_write_dat(0x1a);
						lcd_write_str("Ӣ��");
					lcd_set_pos(1,0); 
						lcd_write_str("2.Ӣ��-");
						lcd_write_dat(0x1a);
						lcd_write_str("����");
					lcd_set_pos(2,0); 
						lcd_write_str("3.ת�����");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("����:");
						
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
								b_shuzhi=a_shuzhi/2.54;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("Ӣ��:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*2.54;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("����:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("Ӣ��:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '2'://���ϣ����� 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.����-");
						lcd_write_dat(0x1a);
						lcd_write_str("����");
					lcd_set_pos(1,0); 
						lcd_write_str("2.����-");
						lcd_write_dat(0x1a);
						lcd_write_str("����");
					lcd_set_pos(2,0); 
						lcd_write_str("3.ת�����");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&&num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("����:");
						
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
								b_shuzhi=5/9*(a_shuzhi-32);
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("����:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*9/5+32;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("����:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("����:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					}
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '3'://1��·��=4.184ǧ��
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.��·��-");
						lcd_write_dat(0x1a);
						lcd_write_str("ǧ��");
					lcd_set_pos(1,0); 
						lcd_write_str("2.ǧ��-");
						lcd_write_dat(0x1a);
						lcd_write_str("��·��");
					lcd_set_pos(2,0); 
						lcd_write_str("3.ת�����");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("��·��:");
						
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
								b_shuzhi=a_shuzhi*4.184;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("ǧ��:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi/4.184;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("��·��:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("ǧ��:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '4'://1Ӣ��=1.54cm 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.Ӣ��/h-");
						lcd_write_dat(0x1a);
						lcd_write_str("����");
					lcd_set_pos(1,0); 
						lcd_write_str("2.����/h-");
						lcd_write_dat(0x1a);
						lcd_write_str("Ӣ��");
					lcd_set_pos(2,0); 
						lcd_write_str("3.ת�����");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("Ӣ��/h:");
						
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
								b_shuzhi=a_shuzhi*1.6092;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("����/h:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*0.621427;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("Ӣ��/h:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("����/h:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
				
				case '5'://�㣬���� 
				a_shuzhi=0;
				b_shuzhi=0;
				do
				{
					lcd_set_pos(0,0); 
						lcd_write_str("1.�Ƕ�-");
						lcd_write_dat(0x1a);
						lcd_write_str("����");
					lcd_set_pos(1,0); 
						lcd_write_str("2.����-");
						lcd_write_dat(0x1a);
						lcd_write_str("�Ƕ�");
					lcd_set_pos(2,0); 
						lcd_write_str("3.ת�����");
					do
					{
						num0=keyboard_output();
					} 
					while(num0!='1' && num0!='2'&& num0!='3'&& num0!=19);
					
					lcd_clear();
					lcd_clear_pic();
					
					switch(num0)
					{
						case '1':
							lcd_set_pos(0,0); 
							lcd_write_str("�Ƕ�:");
						
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
								b_shuzhi=a_shuzhi*0.017453;
							}
							
							break;
							
						case '2':
							lcd_set_pos(0,0); 
							lcd_write_str("����:");
						
							lcd_set_pos(1,0);
							sprintf( temp_buff,"%g",b_shuzhi ); 
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
								b_shuzhi = alg_atof(temp_buff);	
								a_shuzhi=b_shuzhi*57.29578;
							}
							
							break;
						case '3':
							lcd_set_pos(0,0); 
								lcd_write_str("�Ƕ�:");
								lcd_set_pos(1,0); 
								sprintf(temp_buff,"%g",a_shuzhi);
								lcd_write_str(temp_buff);
							lcd_set_pos(2,0); 
								lcd_write_str("����:");
								lcd_set_pos(3,0); 
								sprintf(temp_buff,"%g",b_shuzhi);
								lcd_write_str(temp_buff);
							break; 
					} 
					if(num0==19)
						break; 
				}while(keyboard_output()!=19); 
		}
	
	} 	
}
	


