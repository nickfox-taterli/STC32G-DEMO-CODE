#include "nor_cal.h"
#include "middle.h"
#include "string.h"
#include "keyboard.h"
#include "fun.h"

#define NULL ((void *) 0)       //��ָ��
#define IBlength 26             //�������ݸ���������
u8 xdata SaveBuff[512] ;        //��ʷ��¼��������
 

void normal_calculate(u8 keynum)        //��ͨ����
{
    static u8 xdata addr=0;                     //��ǰ�����ʾ��ַ
    static u8 xdata momory = 0;                 //����������ַ
    u8 xdata result=0;                          //���������ַ������Ƚ�� 0xff��ʾ����ָ�� 
    static u8 xdata shift_flag=0;               //�ڶ����ܱ�־λ
    double xdata re_result;                     //������
	static char xdata input_buf[IBlength+1];    //���뻺��
	char xdata output_buf[16];                  //�������   
    u16 xdata i=0;
    
    switch( state )
    {
        case 0:                                 //��ʼ��
            memset(input_buf,'\0',IBlength);
            memset(output_buf,'\0',16);
            Screen_Clear();
 
        //-----------------------��ȡ�������--------------------
            for(i=0;i<512;i++)
                SaveBuff[i] = IapRead(0x0200+i); //P1=0x12
            if( SaveBuff[0]!= 0x05 )
            {
                memset(SaveBuff,0,512);
                SaveBuff[0] = 0x05;
            }
            
            addr = 0;
            state++;
            break;
     
        case 1://�ȴ�����
a:          Cursor_Blink((u8)(addr*8),0,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )
                {
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )
                {
                    if( input_buf[addr]!=0 )
                        addr+=1;
                }
                else if( keynum == 4 )  //��ʷ��¼�Ϸ�
                {
                    state = 10;
                    momory = 0;
                    goto ad;
                }   
                else if( keynum == 9 )  //��ʷ��¼�·�
                {
                    state = 10;
                    momory = 0;
                    keynum = 4;     //ǿ�ƴӵ�һ����俪ʼ�鿴
                    goto ad;
                }                   
                else if( keynum == 21 )
                {
                    Screen_Clear();     //���ڻ���������һλ�ĺ���
                    String_Delete(&input_buf,IBlength,addr);
                    if( addr>0 )
                        addr--;
                }                 
                else if( keynum == 22 )
                {
                    memset(input_buf,'\0',IBlength);
                    memset(output_buf,'\0',16);                    
                    Screen_Clear();
                    addr=0;
                }   
                else if( keynum == 17 )
                {                  
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),"1.sinh");
                    Screen_ShowStr(0,(u8)(2),"2.cosh");
                    Screen_ShowStr(0,(u8)(4),"3.tanh");
                    state = 5;
                }                   
                else if( keynum == '=' )        //Ӌ��Y��
                {
                    addr = String_length(input_buf);
                    input_buf[addr]='\0';
                    state = 2;
                }    
                else if( keynum == 1 )        //��������
                {
                    shift_flag = !shift_flag;
                }                  
                else
                {
                    if( shift_flag )
                    {
                        keynum += 0x80;
                        shift_flag = 0;
                    }
                    result=String_Insert(&input_buf,IBlength,addr,keynum);
                    if( result!=0xff )
                    {
                        addr += result;
                        if( addr>=IBlength )
                        {
                            addr=IBlength;
                            input_buf[IBlength]='\0';
                        }
                    }                    
                } 
                if( state==1 )
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
        case 2://������
            if( alg_calc_expression(input_buf, &re_result, NULL)==1 )
            {
                sprintf(output_buf,"%g",re_result);
                g_ans = re_result;
                Screen_ShowStr(0,6,output_buf);
                Screen_ShowFreshen();
                    
//------------------------------��ʷ��������------------------------------
                //������ʵ������ѭ������
                if( SaveBuff[1]>=10 )       //���ݴ���10��
                {
                    for(i=0;i<(26*9);i++)        //
                        SaveBuff[4+i] = SaveBuff[4+26+i];
                    SaveBuff[1] = 9;
                }
                if( SaveBuff[1]<10 )
                {
                    for(i=0;i<26;i++)        //һ����ʽ26���ַ�
                    {
                        SaveBuff[4+26*SaveBuff[1]+i] = input_buf[i];
                    }
                    SaveBuff[1]++;
                    IapErase(0x0200);
                    for(i=0;i<512;i++)
                        IapProgram(0x0200+i, SaveBuff[i]);                    
                } 
                        
            }
            else
            {
                Screen_ShowStr(0,6,"INPUT ERROR");
                Screen_ShowFreshen();
            }
            state = 4;
            break;
        case 3://��ʾ��ʷ����
            break;
        
        case 4://�ȴ���һ������
            if( keynum>0 )
            {
                state= 1;  
                memset(input_buf,'\0',IBlength);
                memset(output_buf,'\0',16);
                Screen_Clear(); 
                addr = 0;                
                goto a;
            }  
            break;
        case 5://���⹦��ݔ��
            if( keynum>0 )
            {
                if(( keynum=='1' )||( keynum=='2' )||( keynum=='3' ))
                {
                    addr +=String_Insert(&input_buf,IBlength,addr,120+keynum-'1'); 
                    if( addr>=IBlength )
                    {
                        addr=IBlength;
                        input_buf[IBlength]='\0';
                    }                    
                    state=1;
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                    Screen_ShowFreshen();
                }     
            }                
            break;
        case 10://ӛ���@ʾ
            if( keynum>0 )
            {
                if(( keynum!=4 )&&( keynum!=9 ))
                {
                    Screen_Clear();
                    state = 1;
                    goto a;
                }
                    
ad:             if( SaveBuff[1]==0 )
                {
                    Screen_ShowStr(0,(u8)(0),"û�м���");
                    Screen_ShowFreshen();                
                }
                else
                {
                    if(( keynum==4 ))  
                    {
                        if( ( momory>0 ) )
                            momory--;
                    }
                    else if(( keynum==9  )) 
                    {
                        if( momory<(SaveBuff[1]-1))
                            momory++;
                    }
                    for( i=0;i<26;i++ )
                        input_buf[i] = SaveBuff[momory*26+4+i];
                    
                    alg_calc_expression(input_buf, &re_result, NULL);
                    sprintf(output_buf,"%g",re_result);  
                    
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                    Screen_ShowStr(0,6,output_buf); 
                    Screen_ShowFreshen();
                }
            }                
            break;            
        
    }
}


void complex_calculate(u8 keynum)
{
    static u8 addr;
    u8 result;
    static u8 shift_flag=0;
    double re_result,im_result;       //������
	char input_buf[IBlength];     //���뻺��
	char output_buf[16];    //�������   
    P21=!P21;
    switch( state )
    {
        case 0://��ʼ��
            memset(input_buf,'\0',IBlength);
            memset(output_buf,'\0',16);
            Screen_Clear();
            addr = 0;
            state++;
            break;
     
        case 1://�ȴ�����
a:          Cursor_Blink((u8)(addr*8),0,50); 
            if( keynum>0 )
            {
                if( keynum == 8 )
                {
                    //Screen_ShowFreshen();
                    if(addr>=1)
                        addr-=1;
                }
                else if( keynum == 10 )
                {
                    //Screen_ShowFreshen();
                    if( input_buf[addr]!=0 )
                        addr+=1;
                } 
                else if( keynum == 21 )
                {
                    //Screen_ShowFreshen();
                    Screen_Clear();     //���ڻ���������һλ�ĺ���
                    String_Delete(&input_buf,IBlength,addr);
                    if( addr>0 )
                        addr--;
                }                 
                else if( keynum == 22 )
                {
                    memset(input_buf,'\0',IBlength);
                    memset(output_buf,'\0',16);                    
                    Screen_Clear();
                    addr=0;
                }   
                else if( keynum == 17 )
                {                  
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),"1.sinh");
                    Screen_ShowStr(0,(u8)(2),"2.cosh");
                    Screen_ShowStr(0,(u8)(4),"3.tanh");
                    state = 5;
                }                   
                else if( keynum == '=' )        //Ӌ��Y��
                {
                    addr = String_length(input_buf);
                    input_buf[addr]='\0';
                    state = 2;
                }  
                else if( keynum == 1 )        //��������
                {
                    shift_flag = !shift_flag;
                }                     
                else
                {
                    if( shift_flag )
                    {
                        keynum += 0x80;
                        shift_flag = 0;
                    }                   
                    result=String_Insert(&input_buf,IBlength,addr,keynum);
                    if( result!=0xff )
                    {
                        addr += result;
                        if( addr>=IBlength )
                        {
                            addr=IBlength;
                            input_buf[IBlength]='\0';
                        }                        
                    }                    
                } 
                if( state==1 )
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                Screen_ShowFreshen();
                keynum = 0;
            }
            
            break;
        case 2://������
            if( alg_calc_expression(input_buf, &re_result, &im_result)==1 )
            {
				if(im_result <0.0)
					sprintf(output_buf,"%g%gi",re_result,im_result);
				else
					sprintf(output_buf,"%g+%gi",re_result,im_result);
                
                Screen_ShowStr(0,6,output_buf);
                Screen_ShowFreshen();
            }
            else
            {
                Screen_ShowStr(0,6,"INPUT ERROR");
                Screen_ShowFreshen();
            }
            state = 4;
            break;
        case 3://��ʾ��ʷ����
            break;
        
        case 4://�ȴ���һ������
            if( keynum>0 )
            {
                state= 1;  
                memset(input_buf,'\0',IBlength);
                memset(output_buf,'\0',16);
                Screen_Clear(); 
                addr = 0;                
                goto a;
            }  
            break;
        case 5://���⹦��ݔ��
            if( keynum>0 )
            {
                if(( keynum=='1' )||( keynum=='2' )||( keynum=='3' ))
                {
                    addr +=String_Insert(&input_buf,IBlength,addr,120+keynum-'1');  
                    state=1;
                    if( addr>=IBlength )
                    {
                        addr=IBlength;
                        input_buf[IBlength]='\0';
                    }                    
                    Screen_Clear();
                    Screen_ShowStr(0,(u8)(0),&input_buf);
                    Screen_ShowFreshen();
                }     
            }                
            break;
        
    }
}
