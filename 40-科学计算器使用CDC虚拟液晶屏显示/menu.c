#include "menu.h"
#include "nor_cal.h"
#include "fun.h"
#include "matrix.h"
#include "funcpic.h"
#include "Solve.h"
#include "Sensus.h"
#include "calculus.h"
#include "set.h"

void  (*function)(u8 num);			//ϵͳ��ָ�뺯��
struct MenuItem *P_menu;     //��ǰ�˵��ڵ��ָ��
struct MenuIndex P_Index=	 //��ǰ�˵��ĵ�����Ϣ
{
	0,
	{0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
	0			//Ĭ�ϰ���ΪΪ�˵��л�����
};

void Nop(u8 num) 
{}

struct MenuItem Menu_Main[9];       //���˵�
struct MenuItem Menu_Other[9];      //�����˵�
struct MenuItem Menu_Matrix[10];     //����˵�ѡ�� 
struct MenuItem Menu_Matrix1[4];    //����˵�1
struct MenuItem Menu_Matrix2_4[5];  //����˵�2-4
struct MenuItem Menu_Matrix5_9[4];  //����˵�5-9 
struct MenuItem Menu_FuncImg[7];    //����ͼ����� 
struct MenuItem Menu_ODEFunc[6];    //��΢�ַ��̽���    
struct MenuItem Menu_Sensus[4];
struct MenuItem Menu_Sensus1[3];
	
	
//�ṹ��ʵ�� 
struct MenuItem Menu_Main[9]= 
{ 
    {8,"1.��ͨ����",Menu_MainFunc1,Null,Null}, 
    {8,"2.����\xfd����",Menu_MainFunc2,Null,Null}, 
    {8,"3.��������",Nop,Menu_Matrix,Null}, 
    {8,"4.����\xfdͼ��",Nop,Menu_FuncImg,Null}, 
    {8,"5.�ⷽ��",Menu_MainFunc5,Null,Null}, 
    {8,"6.ͳ��",Nop,Menu_Sensus,Null}, 
    {8,"7.һ�׳�΢�ַ���",Nop,Menu_ODEFunc,Null}, 
    {8,"8.����\xfd����",Nop,Menu_Other,Null},  //ʱ����ʾ
//    {9,"9.Ԥ��",Nop,Null,Null},  //��������
};

struct MenuItem Menu_Matrix[10]= 
{ 
    {10,"1.��������ʽ",Nop,Menu_Matrix1,Menu_Main}, 
    {10,"2.�������ӷ�",Nop,Menu_Matrix2_4,Menu_Main}, 
    {10,"3.����������",Nop,Menu_Matrix2_4,Menu_Main}, 
    {10,"4.�������˷�",Nop,Menu_Matrix2_4,Menu_Main}, 
    {10,"5.���㷽��ߴ���",Nop,Menu_Matrix5_9,Menu_Main}, 
    {10,"6.��������",Nop,Menu_Matrix5_9,Menu_Main}, 
    {10,"7.��ת�þ���",Nop,Menu_Matrix5_9,Menu_Main}, 
    {10,"8.��������",Nop,Menu_Matrix5_9,Menu_Main},  
    {10,"9.����������\xfd��",Nop,Menu_Matrix5_9,Menu_Main},  
    {10,"0.����",Nop,Nop,Menu_Main}, 
};
struct MenuItem Menu_Matrix1[4]= 
{ 
    {4,"1.����ʽ��",Menu_MainFunc31,Null,Menu_Matrix}, 
    {4,"2.�鿴������",Menu_MainFunc31,Null,Menu_Matrix}, 
    {4,"3.��������ʽ",Menu_MainFunc31,Null,Menu_Matrix}, 
    {4,"0.����",Nop,Null,Menu_Matrix}, 
};
struct MenuItem Menu_Matrix2_4[5]= 
{ 
    {5,"1.����Ľ�",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"2.�޸ĵ�һ����",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"3.�޸ĵڶ�����",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"4.�������",Menu_MainFunc32,Null,Menu_Matrix}, 
    {5,"0.����",Nop,Null,Menu_Matrix},     
};
struct MenuItem Menu_Matrix5_9[4]= 
{ 
    {4,"1.���þ�������",Menu_MainFunc33,Null,Menu_Matrix}, 
    {4,"2.�鿴���޸ľ���",Menu_MainFunc33,Null,Menu_Matrix}, 
    {4,"3.      ",Menu_MainFunc33,Null,Menu_Matrix}, 
    {4,"0.����",Menu_MainFunc33,Null,Menu_Matrix}, 
};

struct MenuItem Menu_FuncImg[7]= 
{ 
    {7,"1.ԭ������X0",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"2.ԭ������y0",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"3.�����굥λ����",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"4.�����굥λ����",Menu_MainFunc4,Null,Menu_Main},
    {7,"5.ͼ����ʽ",Menu_MainFunc4,Null,Menu_Main}, 
    {7,"6.��ʾͼ��",Menu_MainFunc4,Null,Menu_Main},   
    {7,"0.����",Nop,Null,Menu_Main},      
};

struct MenuItem Menu_Sensus[4]= 
{ 
    {4,"1.������\xfd�ݸ���\xfd",Menu_MainFunc6,Null,Menu_Main}, 
    {4,"2.������\xfd��",Menu_MainFunc6,Null,Menu_Main}, 
    {4,"3.����",Nop,Menu_Sensus1,Menu_Main},   
    {4,"0.����",Nop,Null,Menu_Main},      
};
struct MenuItem Menu_Sensus1[3]= 
{ 
    {3,"1.�鿴��\xfd�ݽ��",Menu_MainFunc61,Null,Menu_Sensus}, 
    {3,"2.�鿴����\xfd",Menu_MainFunc61,Null,Menu_Sensus},  
    {3,"0.����",Nop,Null,Menu_Main},      
};


struct MenuItem Menu_ODEFunc[6]= 
{ 
    {6,"1.�ұߵĺ���\xfd��",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"2.��˵�a",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"3.�Ҷ˵�b",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"4.����h",Menu_MainFunc7,Null,Menu_Main},  
    {6,"5.��˵��y ֵ",Menu_MainFunc7,Null,Menu_Main}, 
    {6,"6.���",Menu_MainFunc7,Null,Menu_Main},      
};


//�ṹ��ʵ�� 
struct MenuItem Menu_Other[9]= 
{ 
    {6,"1.��Ļ����",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"2.shift ��ʾ",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"3.�ڵ�����",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"4.��������",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"5.ʱ������",Menu_MainFunc8,Null,Menu_Main}, 
    {6,"6.����",Menu_MainFunc8,Null,Menu_Main}, 
};

//------------------------------------------------------------------�˵���ʼ�����˵��л�������ִ�еȵĵĲ�������-------------------------------------------------------------
void Menu_Init(void)
{
    P_menu = &Menu_Main[0];      //ָ��ָ���ʼ�ڵ� 
	P_Index.Menu_Depth = 0;	//�˵����Ϊ0
    function = Nop;
    P_Index.Menu_KeyFunc = 0;   //�ˆ�̎��
//    Menu_Display();//��ʾ������ʾ�˵�
}

void Menu_Display(void)
{
	int i,j;
	
	j = P_Index.Menu_Start[P_Index.Menu_Depth];		//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();

                
    for( i=0;i<4;i++ )
    {
        if( i<P_menu[0].MenuCount )
            Screen_ShowStr(0,(u8)(i*2),P_menu[i+j].DisplayString);
        else 
            break;
        
    }    
    if( P_Index.Menu_Depth==2 )    //ǿ����ʾ
    {
        switch( P_Index.Menu_Pilot[P_Index.Menu_Depth-1] )
        {
            case 4:Screen_ShowStr(16,4,"����η�n");
                   Screen_ShowStr(0,6,"4.����  0.����");break;
            
            case 5:Screen_ShowStr(16,4,"�鿴�������");break;
            case 6:
                Screen_ShowStr(16,4,"�鿴ת�������");break;
            case 7:Screen_ShowStr(16,4,"�鿴�������");break;  
            case 8:Screen_ShowStr(16,4,"������\xfd��ϵ��\xfdk");
                    Screen_ShowStr(0,6,"4.����  0.����");
                    break; 
        }
        //Screen_ShowFreshen();
    }
    Screen_ShowFreshen();
}

void Run(struct MenuItem *manyou) 
{ 
	(*(manyou->Subs))(); 
}

void Menu_Deal(char* key)
{
    u8 keyval = *key;
    
    if( P_Index.Menu_KeyFunc==0 )		//�˵��л�
    {
        if( keyval >0 )
        {
            if( keyval==4 ) //�Ϸ�
            {
                if( P_Index.Menu_Start[P_Index.Menu_Depth]>0 )
                    P_Index.Menu_Start[P_Index.Menu_Depth]--;
            }
            else if( keyval==9 ) //�·�
            {
                if( P_Index.Menu_Start[P_Index.Menu_Depth]+4<P_menu[0].MenuCount )
                    P_Index.Menu_Start[P_Index.Menu_Depth]++;		//ָ��             
            } 
            else if(( keyval==22 )||( keyval=='0' )) //����
            {
                unsigned char p =  P_Index.Menu_Start[P_Index.Menu_Depth];		//���㵱ǰѡ�е���ı��
                if( P_menu[p].Parentms != Null )
                {
                    P_Index.Menu_Start[P_Index.Menu_Depth]=0;
                    P_Index.Menu_Pilot[P_Index.Menu_Depth]=0;
                    
                    P_Index.Menu_Depth--;
                    P_menu = P_menu[p].Parentms;
                }  

            } 
            else if(( keyval >='1' )&&( keyval<='9' )) //����
            {
               // if( (u8)(keyval-'0')<(u8)P_menu[P_Index.Menu_Depth-1].MenuCount )
                {                    
                    unsigned char p = keyval-'1' ;		//���㵱ǰѡ�е���ı��
                    
                    if( P_menu[p].Childrenms != Null )
                    {
                        P_Index.Menu_Pilot[P_Index.Menu_Depth]=p;
                        P_Index.Menu_Depth++;
                        P_menu = P_menu[p].Childrenms;
                    }
                    else if( P_menu[p].Subs != Null )
                    { 
                        P_Index.Menu_Pilot[P_Index.Menu_Depth]=p;
                        Run( &P_menu[p]);
                    }  
                }                
            }     
            if( P_Index.Menu_KeyFunc==0 )
            {
                Menu_Display();
            }
            *key = 0;            
        }
        if( state>0 )
        {
            Menu_Display();
            state = 0;
        }
    }
    else
    {
        if( keyval==5 )
        {
            Menu_Display();  
            P_Index.Menu_KeyFunc=0;
        }
        else
            function(keyval);
        *key = 0;
    }
}

//------------------------------------------------------------------�˵���ʼ���Լ��˵��л��Ĳ�������-------------------------------------------------------------
void Menu_MainFunc1(void)
{
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_NORMAL;
	function = normal_calculate;
}

void Menu_MainFunc2(void)
{
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_COMPLEX;
	function = complex_calculate;
}

void Menu_MainFunc31(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    
    if( address==0 )
    {
        function = Ranks_Set;
    }
    else if( address==1 )
    {
        function = Ranks_Amend;
    }
    else if( address==2 )
    {
        function = Ranks_Caculate;
    }
}

void Menu_MainFunc32(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    
    if( address==0 )
    {
        function = Ranks_2_4Set;
    }
    else if( address==1 )
    {
        function = Ranks_AmendA;
    }
    else if( address==2 )
    {
        function = Ranks_AmendB;
    }
    else if( address==3 )
    {
        function = Ranks_Caculate2;
    }    
}

void Menu_MainFunc33(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    
    if( address==0 )
    {
        if(( P_Index.Menu_Pilot[P_Index.Menu_Depth-1]==6 )
            ||( P_Index.Menu_Pilot[P_Index.Menu_Depth-1]==7 )
            ||( P_Index.Menu_Pilot[P_Index.Menu_Depth-1]==8 )
        )
            function = Ranks_2_4Set;       //ݔ���к��Ѓɂ���ֵ
        else
            function = Ranks_Set;       //ݔ��1-5n
    }
    else if( address==1 )
    {
        function = Ranks_AmendA;        //�������
    }
    else if( address==2 )
    {
        switch( P_Index.Menu_Pilot[P_Index.Menu_Depth-1] )
        {
            case 4:function = Ranks_Setpower;break;     //�η�ݔ��
            case 5:function = Ranks_Caculate3;break;   //�鿴�������
            case 6:function = Ranks_Caculate3;break;//�鿴ת�������
            case 7:function = Ranks_changeCaculate;break;//�鿴�������
            case 8:function = Ranks_Setpower;break;     //��������ϵ��k
            default:function = Nop;break;
        }
    }
    else if( address==3 )
    {
        switch( P_Index.Menu_Pilot[P_Index.Menu_Depth-1] )
        {
            case 4:function = Ranks_Caculate3;break;
            case 8:function = Ranks_Caculate3;break;
            default:function = Nop;break;
        }
        //function = Ranks_Caculate2;
    }    
}

void Menu_MainFunc4(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_NORMAL;
    
    if( address==0 )
    {
        function = FuncPic_SetX0;
    }
    else if( address==1 )
    {
        function = FuncPic_SetY0;
    }
    else if( address==2 )
    {
        function = FuncPic_SetKX;
    }
    else if( address==3 )
    {
        function = FuncPic_SetKY;
    }    
    else if( address==4 )
    {
        function = FuncPic_Write;
    }   
    else if( address==5 )
    {
        function = FuncPic_ShowPic;
    }        
}


void Menu_MainFunc5(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    function = Solve;    
}

void Menu_MainFunc6(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    if( address==0 )
        function = Sensus_Set;  
    else if( address==1 )
        function =  Sensus_Amend;
}
void Menu_MainFunc61(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    if( address==0 )
        function = Sensus_Check;  
    else if( address=1 )
    function =  Sensus_Check2;
}

void Menu_MainFunc7(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;
    g_chCalcStatus = CALC_NORMAL; 
    if( address==0 )
        function = calculus_funcWR;  
    else if( address==1 )
        function =  calculus_funcWRa;
    else if( address==2 )
        function =  calculus_funcWRb;
    else if( address==3 )
        function =  calculus_funcWRh;
    else if( address==4 )
        function =  calculus_funcWRy;    
    else if( address==5 )
        function =  calculus_funcShow;
    
}
void Menu_MainFunc8(void)
{
    unsigned char address = P_Index.Menu_Pilot[P_Index.Menu_Depth];	//��ǰҳ�濪ʼ�ĵ�ѡ��
	Screen_Clear();
	P_Index.Menu_KeyFunc = 1;
    state = 0;

    if( address==0 )
        function = Set_Light;  
    else if( address==1 )
        function =  Set_LightShift;
    else if( address==2 )
        function =  Set_Sleep;
    else if( address==3 )
        function =  Set_Data;
    else if( address==4 )
        function =Set_Time  ;    
//    else if( address==5 )
//        function =  calculus_funcShow;
    
}

