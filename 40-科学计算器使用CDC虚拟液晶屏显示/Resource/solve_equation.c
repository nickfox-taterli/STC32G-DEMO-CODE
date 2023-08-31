#include <stdio.h>
#include <math.h>
#include "middle.h"
#include "keyboard_device.h"
#include "alg_atof.h"
#include "solve_equation.h"
#include "alg_caculate.h"
extern uchar g_chCalcStatus;
//#include <stdlib.h> 

//a+b*x+c*x^2+d*x^3+e*x^4...

extern uchar num0;

double F1(double x,double a,double b,double c,double d,double e)//ԭ���� 
{
	return  e + d*x + c*x*x + b*x*x*x + a*x*x*x*x; 
}

double F2(double x,double a,double b,double c,double d)//ԭ������һ�׵�����
{
	return  d + 2*c*x + 3*b*x*x + 4*a*x*x*x; 
}

double Newton(double x0,double precision,double a,double b,double c,double d,double e,uchar* x)//ţ�ٷ� 
{
	double x1;
	uchar wrong[6]  = "wrong";//����wrong���ַ�����5+1����Ԫ 
	uint solve_equation_count=0;
	if(x0==0.0 )
	{
		if(F1(x0,a,b,c,d,e)==0.0)
		{
			sprintf(x,"%g",x0); 
			return 1;
		}
		else
			x0=x0+0.000001;
	}
	
	do
	{
	 x1 = x0;
	 x0 = x1 - F1(x1,a,b,c,d,e) / F2(x1,a,b,c,d);
	 solve_equation_count++;
	 if( solve_equation_count >1000)
	 {
		uchar i=0;
		for(i=0;i<5;i++)
		{
			x[i]=wrong[i];
		}
		x[i]='\0';//�ǵø��ַ���x���Ͻ�β�� 0 
	 
	 	return 0;
	 }
	 	
	}
	while (fabs(x1 - x0) > precision  );
	sprintf(x,"%g",x0); 
	return 1;
}

void read_keyboard(uchar *str,uchar length,double *shuzhi)//�����ַ�����ָ��ͳ���
							//��ȡ���̵���ֵ��ֱ�����¡����ߡ�����= ,
{							//����num0��ȫ�ֱ���������'='�ᱻ������������ЧӰ������ж�  
	uchar  count_numbers=0;//��¼�����˼�����Ч���ֻ����
	num0 = keyboard_output();
	while( num0!= 5 && num0!= 7 && num0!='=' ) 	//ѭ��ֱ�����¡����ߡ�����= 
	{
		if( count_numbers<(length) )
		{
			if( ( num0>='0' && num0<='9' ) || num0=='.' || num0=='-' )
			{
				if(count_numbers==0)
				{
					for(length=0;length<16;length++)
					{
						str[length]=' ';
					}
					str[length-1]=0;
				}
				str[count_numbers]=num0;
				count_numbers++;
			}
			else if(num0==18) //�����DEL��
			{
				if(count_numbers > 0)
				{
					count_numbers--;
					str[count_numbers]=' ';
				}	
			}
//			lcd_set_pos(3,0);
//			lcd_write_str("                ");
			lcd_set_pos(3,0);
			lcd_write_str(str);
		}
		num0 = keyboard_output();
	}
	if(count_numbers>0)
	{
		*shuzhi=alg_atof(str);
		sprintf(str,"%g",*shuzhi);
	}
} 


void solve_equation()
{	
	int n;	//��߽���
	uchar page_i = 0;
	
	uchar temp_buff[17];//�������ϵ�����ַ���,��ʾ10��+��β1�� 
//	uchar b[17];
//	uchar c[17];
//	uchar d[17];
//	uchar e[17];
//	uchar x0[17];//������ƽ�x0���ַ���
//	uchar x[17];// �����x���ַ���
	uchar wrong[6]  = "wrong";//����wrong���ַ�����5+1����Ԫ 
	double a_shuzhi = 0;
	double b_shuzhi = 0;
	double c_shuzhi = 0;
	double d_shuzhi = 0;
	double e_shuzhi = 0;
	double x0_shuzhi= 0;
	double x_shuzhi = 0;	//x����õĸ� 

	uchar i;
	g_chCalcStatus = CALC_NORMAL;
	for(i=0;i<16;i++)
	{
		temp_buff[i]=' ';
	}
	temp_buff[16]=0;

	lcd_clear();
	lcd_clear_pic();
	lcd_set_pos(0,0);
	lcd_write_str("y=aX^4+bX^3+cX^2");//��0�к͵�һ����ʾ���� 
	lcd_set_pos(1,1);
	lcd_write_str("+dX+e     OK?=");
	
	
//�������ϵ�� 
	num0=0xff;
	while( num0 != '=' )//������Ĳ���'='����һֱɨ����̵�����,
	{					//while�����ж�������'1','2'���ߡ� ��,�������뱻����  
		switch( page_i )
		{
			case 0:
			
				lcd_set_pos(2,0);
				lcd_write_str("a =           ");
				lcd_write_dat(' ');
				lcd_write_dat(0x1a);// ��
				
				lcd_set_pos(3,0);
				sprintf(temp_buff,"%g",a_shuzhi);
				lcd_write_str(temp_buff);

				read_keyboard(temp_buff,17,&a_shuzhi);//����read_keyboard��������һֱɨ�����,//ֱ�����¡�������'=' 		
				break;
			case 1:
				lcd_set_pos(2,0);
				lcd_write_str("b =           ");
				lcd_write_dat(0x1b);//��
				lcd_write_dat(0x1a);//��
				
				lcd_set_pos(3,0);
				sprintf(temp_buff,"%g",b_shuzhi);
				lcd_write_str(temp_buff);

				read_keyboard(temp_buff,17,&b_shuzhi);//����read_keyboard��������һֱɨ�����,//ֱ�����¡�������'=' 					
				break;
			case 2:
				lcd_set_pos(2,0);
				lcd_write_str("c =           ");
				lcd_write_dat(0x1b);//��
				lcd_write_dat(0x1a);//��
				lcd_set_pos(3,0);
				sprintf(temp_buff,"%g",c_shuzhi);
				lcd_write_str(temp_buff);

				read_keyboard(temp_buff,17,&c_shuzhi);//����read_keyboard��������һֱɨ�����,//ֱ�����¡�������'='  	
				break;
			case 3:
				lcd_set_pos(2,0);
				lcd_write_str("d =           ");
				lcd_write_dat(0x1b);//��
				lcd_write_dat(0x1a);//��
				lcd_set_pos(3,0);
				sprintf(temp_buff,"%g",d_shuzhi);
				lcd_write_str(temp_buff);

				read_keyboard(temp_buff,17,&d_shuzhi);//����read_keyboard��������һֱɨ�����,//ֱ�����¡�������'=' 
				break;
			case 4:
				lcd_set_pos(2,0);
				lcd_write_str("e =           ");
				lcd_write_dat(0x1b);//��
				lcd_write_dat(0x1a);//��
				lcd_set_pos(3,0);
				sprintf(temp_buff,"%g",e_shuzhi);
				lcd_write_str(temp_buff);

				read_keyboard(temp_buff,17,&e_shuzhi);//����read_keyboard��������һֱɨ�����,//ֱ�����¡�������'=' 
				break;
			case 5:
				lcd_set_pos(2,0);
				lcd_write_str("x0=           ");
				lcd_write_dat(0x1b);//��
				lcd_write_dat(' ');//��
				lcd_set_pos(3,0);
				sprintf(temp_buff,"%g",x0_shuzhi);
				lcd_write_str(temp_buff);

				read_keyboard(temp_buff,17,&x0_shuzhi);//����read_keyboard��������һֱɨ�����,//ֱ�����¡�������'=' 
				break;
		}
		
		lcd_set_pos(3,0);
		lcd_write_str("                ");//�����������ֵ��׼���ٴ�ѭ���������˳���ʾ�����ʱ��ˢ��

	//	num0 = keyboard_output();//�ж������ǡ����ߡ�,
								//�����'='����һ�δ�ѭ��ʱҲ��ʶ���������뱻���� 
		switch (num0)
		{
				
			case 5://�� ������һҳ 
				
				if(page_i<=0)
					page_i=0;
				else  
					page_i--; 
				break;
			case 7://�� ������һҳ 
				if(page_i>=5)
					page_i=5;
				else  
					page_i++; 
				break;
		}
	} 
	
//��ʾ���
	//����������
	lcd_clear(); 
	 
	lcd_set_pos(0,0);
	lcd_write_str("y=aX^4+bX^3+cX^2");//��0�к͵�һ����ʾ���� 
	lcd_set_pos(1,1);
	lcd_write_str("+dX+e");
	
	//�ж�����ݴ� 
	if( a_shuzhi !=0 )
	{
		n=4;
	}
	else if( b_shuzhi !=0 )
	{
		n=3;
	}
	else if( c_shuzhi !=0 )
	{
		n=2;
	}
	else if( d_shuzhi !=0 )
	{
		n=1;
	}
	else
	{
		n=0;
	}
	//��������ݴ�ѡ���㷨 
	if(n==0)
	{	
		uchar i=0;
		for(i=0;i<5;i++)
		{
			temp_buff[i]=wrong[i];
		}
		temp_buff[i]='\0';//�ǵø��ַ���x���Ͻ�β�� 0 
		
	}
	else if(n==1)
	{
		x_shuzhi = e_shuzhi/d_shuzhi;
		sprintf(temp_buff,"%g",x_shuzhi);
	}	 
	else if(n==2)
	{
		if( ( d_shuzhi * d_shuzhi -4 * c_shuzhi * e_shuzhi ) <0  )
		{
			uchar i=0;
			for(i=0;i<5;i++)
			{
				temp_buff[i]=wrong[i];
			}
			temp_buff[i]='\0';//�ǵø��ַ���x���Ͻ�β�� 0 
		}		
		else
		{
			x_shuzhi = (-d_shuzhi+sqrt(d_shuzhi*d_shuzhi - 4*c_shuzhi*e_shuzhi)) / (2*c_shuzhi) ;//x2
			sprintf(temp_buff,"%g",x_shuzhi);
			lcd_set_pos(3,0);
			lcd_write_str("x2 =");
			lcd_write_str(temp_buff); 
			
			x_shuzhi = (-d_shuzhi-sqrt(d_shuzhi*d_shuzhi - 4*c_shuzhi*e_shuzhi)) / (2*c_shuzhi) ;//x1
			sprintf(temp_buff,"%g",x_shuzhi); 
		}
			
	}
	else 
	{	
	    Newton(x0_shuzhi,0.00001,a_shuzhi,b_shuzhi,c_shuzhi,d_shuzhi,e_shuzhi,temp_buff);
	    
	}
	
	lcd_set_pos(2,0);	//ͳһ��ʾx1 
	lcd_write_str("x1 =");
	lcd_write_str(temp_buff);
	 
	do{
		num0 = keyboard_output();
	}
	while( num0!= 5 );
} 

   
			
