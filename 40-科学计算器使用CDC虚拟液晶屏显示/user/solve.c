#include "solve.h"
#include "menu.h"
#include "stdlib.h"
#include "fun.h"
#include <math.h>


double a_shuzhi = 0;
double b_shuzhi = 0;
double c_shuzhi = 0;
double d_shuzhi = 0;
double e_shuzhi = 0;
double x0_shuzhi= 0;
double x_shuzhi = 0;	//x����õĸ� 

u8 solvestr[12];
u8 solvestrlength=0;
u8 solvestrnum=0;

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
//		uchar i=0;
//		for(i=0;i<5;i++)
//		{
//			x[i]=wrong[i];
//		}
//		x[i]='\0';//�ǵø��ַ���x���Ͻ�β�� 0 
	 
	 	return 0;
	 }
	 	
	}
	while (fabs(x1 - x0) > precision  );
	sprintf(x,"%g",x0); 
	return 1;
}

void Solve(u8 keynum)
{
    int n;	//��߽���
    uchar temp_buff[17];
    switch( state )
    {
        case 0://��ʼ��
            Screen_Clear();
            Screen_ShowStr(0,0,"y=aX^4+bX^3+cX^2");
            Screen_ShowStr(16,2,"+dX+e     OK?=");
            Screen_ShowStr(0,4,"a =            >");
            Screen_ShowNum(0,6,a_shuzhi);
            Screen_ShowFreshen();
        
            state = 1;
            solvestrlength  =  0;
            memset(solvestr,'\0',12);
            solvestrnum = 0;
            break;
     
        case 1://�ȴ�����
            if( keynum>0 )
            {
                if((solvestrnum>0 )&&( keynum==8 ))
                {
                    if( solvestrlength>0)
                    {
                        switch( solvestrnum )
                        {
                            case 0:a_shuzhi = atof(solvestr);break;
                            case 1:b_shuzhi = atof(solvestr);break;
                            case 2:c_shuzhi = atof(solvestr);break;
                            case 3:d_shuzhi = atof(solvestr);break;
                            case 4:e_shuzhi = atof(solvestr);break;
                            case 5:x0_shuzhi = atof(solvestr); break;                           
                        }
                        solvestrlength = 0;
                        memset(solvestr,'\0',12);
                    }
                    solvestrnum--;
                    if( solvestrnum<=4 )
                    {
                        Screen_ShowAsc(0,4,'a'+solvestrnum);
                        Screen_ShowAsc(8,4,' ');
                    }     
                    Screen_ShowStr(0,6,"                ");                    
                    switch( solvestrnum )
                    {
                        case 0:Screen_ShowNum(0,6,a_shuzhi);break;
                        case 1:Screen_ShowNum(0,6,b_shuzhi);;break;
                        case 2:Screen_ShowNum(0,6,c_shuzhi);;break;
                        case 3:Screen_ShowNum(0,6,d_shuzhi);;break;
                        case 4:Screen_ShowNum(0,6,e_shuzhi);;break;
                        case 5:Screen_ShowNum(0,6,x0_shuzhi);; break;                           
                    }                    
                }
                else if(( solvestrnum<5 )&&( keynum==10 ))
                {
 
                    if( solvestrlength>0)
                    {
                        switch( solvestrnum )
                        {
                            case 0:a_shuzhi = atof(solvestr);break;
                            case 1:b_shuzhi = atof(solvestr);break;
                            case 2:c_shuzhi = atof(solvestr);break;
                            case 3:d_shuzhi = atof(solvestr);break;
                            case 4:e_shuzhi = atof(solvestr);break;
                            case 5:x0_shuzhi = atof(solvestr); break;                           
                        }
                        solvestrlength = 0;
                        memset(solvestr,'\0',12);
                    }
                
                    solvestrnum++;
                    if( solvestrnum<=4 )
                    {
                        Screen_ShowAsc(0,4,'a'+solvestrnum);
                        Screen_ShowAsc(8,4,' ');
                    } 
                    else
                    {
                        Screen_ShowAsc(0,4,'x');
                        Screen_ShowAsc(8,4,'0');
                    } 
                    Screen_ShowStr(0,6,"                ");
                    switch( solvestrnum )
                    {
                        case 0:Screen_ShowNum(0,6,a_shuzhi);break;
                        case 1:Screen_ShowNum(0,6,b_shuzhi);;break;
                        case 2:Screen_ShowNum(0,6,c_shuzhi);;break;
                        case 3:Screen_ShowNum(0,6,d_shuzhi);;break;
                        case 4:Screen_ShowNum(0,6,e_shuzhi);;break;
                        case 5:Screen_ShowNum(0,6,x0_shuzhi);; break;                           
                    }                          
                }
                else if((( keynum>='0' )&&( keynum<='9' ))||( keynum=='.' )||( keynum=='-' ))
                {
                    solvestr[solvestrlength++]=keynum;
                    Screen_ShowStr(0,6,solvestr);
                }
                else if( keynum == '=' )
                {        
                    if( solvestrlength>0)
                    {
                        switch( solvestrnum )
                        {
                            case 0:a_shuzhi = atof(solvestr);break;
                            case 1:b_shuzhi = atof(solvestr);break;
                            case 2:c_shuzhi = atof(solvestr);break;
                            case 3:d_shuzhi = atof(solvestr);break;
                            case 4:e_shuzhi = atof(solvestr);break;
                            case 5:x0_shuzhi = atof(solvestr); break;                           
                        }
                        solvestrlength = 0;
                        memset(solvestr,'\0',12);
                    }
                    
                    Screen_Clear(); 
                    state =2;
                }
                else if( keynum == 21 ) //�h��
                {
                    if( solvestrlength>0 )
                    {
                        solvestrlength--;
                        solvestr[solvestrlength]='\0';
                        Screen_ShowStr(0,6,"                ");   
                        Screen_ShowStr(0,6,solvestr);   
                    }                        
                }
                else  if( keynum == 22) //����
                {
                    solvestrlength=0;
                    memset(solvestr,'\0',12);
                    Screen_ShowStr(0,6,"                ");   
                }
                else
                {}
                
                if( state == 1 )  
                {
                    if( solvestrnum>0 )
                        Screen_ShowAsc(14*8,4,'<');
                    else
                        Screen_ShowAsc(14*8,4,' ');
                    
                    if( solvestrnum<5 )
                        Screen_ShowAsc(15*8,4,'>');
                    else
                        Screen_ShowAsc(15*8,4,' ');
                    
                    Screen_ShowFreshen();
                }
            }
            break; 
        case 2://����
            //�ж�����ݴ� 
            Screen_ShowStr(0,0,"y=aX^4+bX^3+cX^2");
            Screen_ShowStr(0,2,"  +dX+e         ");        
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
            if( n==0 )
            {
                Screen_ShowStr(0,0,"parm error");
            }
            else if( n==1 )
            {
                x_shuzhi = e_shuzhi/d_shuzhi;
                sprintf(temp_buff,"%g",x_shuzhi); 
                Screen_ShowStr(0,0,temp_buff);
            }                
            else if(n==2)
            {
                if( ( d_shuzhi * d_shuzhi -4 * c_shuzhi * e_shuzhi ) <0  )
                {
//                    uchar i=0;
//                    for(i=0;i<5;i++)
//                    {
//                        temp_buff[i]=wrong[i];
//                    }
//                    temp_buff[i]='\0';//�ǵø��ַ���x���Ͻ�β�� 0 
                    Screen_ShowStr(0,4,"x1 = wrong");
                }		
                else
                {
                    x_shuzhi = (-d_shuzhi+sqrt(d_shuzhi*d_shuzhi - 4*c_shuzhi*e_shuzhi)) / (2*c_shuzhi) ;//x2
                    sprintf(temp_buff,"x2 =%g",x_shuzhi);
                    Screen_ShowStr(0,6,temp_buff);
//                    lcd_set_pos(3,0);
//                    lcd_write_str("x2 =");
//                    lcd_write_str(temp_buff); 
                    
                    x_shuzhi = (-d_shuzhi-sqrt(d_shuzhi*d_shuzhi - 4*c_shuzhi*e_shuzhi)) / (2*c_shuzhi) ;//x1
                    sprintf(temp_buff,"x1 =%g",x_shuzhi); 
                    Screen_ShowStr(0,4,temp_buff);  
                }
                    
            } 
            else 
            {	
                if(Newton(x0_shuzhi,0.00001,a_shuzhi,b_shuzhi,c_shuzhi,d_shuzhi,e_shuzhi,temp_buff)==0)
                {
                    Screen_ShowStr(0,4,"x1 = wrong");
                }
                else
                {
                    Screen_ShowStr(0,4,"x1 ="); 
                    Screen_ShowStr(32,4,temp_buff); 
                }                    
            }
            Screen_ShowFreshen();
            state = 3;
            break;            
        case 3://����
            if( keynum>0 )
            {
                state = 0;
            }
            break;
    }    
}

