/*
    Mini DSO with STC MCU

    GitHub: https://github.com/CreativeLau/Mini-DSO
    
    Copyright (c) 2020 Creative Lau (creativelaulab@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

     
    Version: V0.3
    MCU Type: STC8A8K64S4A12 @27MHz
    YouTube Tutorial(First Version): https://youtu.be/c6gBv6Jcz7w
    YouTube Tutorial(V0.3): https://youtu.be/-8PadlS7c4c
    Instructable(First Version): https://www.instructables.com/id/Make-Your-Own-OscilloscopeMini-DSO-With-STC-MCU-Ea/
    Instructable(V0.3): 
    Any question please contact CreativeLauLab@gmail.com
    YouTube Channel: https://www.youtube.com/c/CreativeLau

    Note:
    How to Operation:
        Main Interface - Parameter Mode:
            Single Click Encoder: Run/Stop sampling
            Double Click Encoder: Enter Wave Scroll Mode
            Long Press Encoder: Enter Settings Interface
            Rotate Encoder: Adjust parameters
            Rotate Encoder While Pressing: Switch between options
            
        Main Interface - Wave Scroll Mode: 
            Single Click Encoder: Run/Stop sampling
            Double Click Encoder: Enter Parameter Mode
            Long Press Encoder: Enter Settings Interface
            Rotate Encoder: Scroll waveform horizontally (only available when sampling stopped)
            Rotate Encoder While Pressing: Scroll waveform vertically (only available when sampling stopped)
            
        Settings Interface:
            Single Click Encoder: N/A
            Double Click Encoder: N/A
            Long Press Encoder: Return to Main Interface
            Rotate Encoder: Adjust parameters
            Rotate Encoder While Pressing: Switch between options

    Parameters in Main Interface:
        Seconds Per Division: "500ms", "200ms", "100ms", "50ms", "20ms", "10ms","5ms", "2ms", "1ms", "500us", "200us", "100us"(100us only available in Auto Trigger Mode)
        Voltage Range: Maximum voltage is 30V. Rotate Encoder clockwise continuous to enter auto range. Rotate Encoder anticlockwise to enter manual range. 
        Trigger Level: Set trigger voltage level. The trigger position will be put at the center of chart. 
                       You should set a correct Trigger Level in Single and Normal Trigger Mode in order to capture the waveform.
        Trigger Slope: Trigger on Rising or Falling Edge. 
                       You should set a correct Trigger Slop in Single and Normal Trigger Mode in order to capture the waveform.
        Trigger Mode:
            Auto:   Sweep continuous, indicator always on. Single click the encoder to stop or run sampling. 
                    If triggered, the waveform will be shown on the display and the trigger position will be put at the center of chart. 
                    Otherwise, the waveform will scroll irregular, and 'Fail' will be shown on the display.
            Normal: When complete pre-sampling, you can input signal. If triggered, waveform shown on the display and waiting for new trigger. 
                    If no new trigger, the waveform will be kept.
            Single: When complete pre-sampling, you can input signal. If triggered, waveform shown on display and stop sampling. 
                    User need to single click Encoder to start next sampling.
            For Normal Mode and Single Mode, be sure the trigger level has been adjusted correctly, otherwise no waveform will be shown on the display.

    Status in Main Interface:
        'Run': Sampling Running.
        'Stop': Sampling Stopped.
        'Fail': The Trigger Level beyond the waveform in Auto Trigger Mode.
        'Auto': Auto Voltage Range.
                    
    Parameters in Settings Interface:
        PMode(Plot Mode): Show waveform in Vector or Dots.
        LSB(Sampling Coefficient): 100 times of voltage dividing coefficient. e.g. the resistor for voltage dividing is 10k and 2k, calculate the voltage dividing coefficient (10+2)/2=6. Get the LSB=6x100=600.
        BRT(Brightness): Adjust OLED Brightness

    Saving Settings: When exit settings interface, all parameters in settings and main interface will be saved in EEPROM.

    Indicator:
    Generally, the indicator on means the sampling is running. 
    The more important use is in Single and Normal Trigger Mode, before get into the trigger stage, pre-sampling is required. 
    The indicator will not on during pre-sampling stage. We should not input signal until the indicator comes on. 
    The longer time scale selected, the longer waiting time of pre-sampling.

    2020-04-30 Update
    1. Add Single Sweep and Normal Sweep function. Set trigger value to catch the waveform mutation.
    2. According to datasheet, set ADC port to high-impedance.

    2020-05-3 Update
    1. Show frequency of waveform.
    2. Scroll waveform horizontally/vertically with EC11 Encoder.
    3. Zoom waveform vertically with EC11 Encoder.
    4. The trigger level could be customized.

    ����ʾ����
    �汾��V0.3
    ���ߣ�����������
    ��Ƭ���ͣ�STC8A8K64D4 @27MHz
    �κ���������ϵCreativeLauLab@gmail.com
    ���������ģ�ȫ��ͬ�����ڴ����Ĺ�ע��
    Bվ�̳̣����棩��https://www.bilibili.com/video/BV1ai4y1t79R
    Bվ�̳̣�V0.3����https://www.bilibili.com/video/BV1ai4y1t79R
    �͹�Ƶ��: https://www.youtube.com/c/CreativeLau

    ����˵����
        ������-ѡ������ģʽ��
            �������������л���ʼ/��ͣ����
            ˫�����������л�����ˮƽ����ģʽ/ѡ������ģʽ
            �������������������ý���
            ��ת���������޸ĵ�ǰ����
            ����ͬʱ��ת���������ڲ������л�
            
        ������-����ˮƽ����ģʽ��
            �������������л���ʼ/��ͣ����
            ˫�����������л�����ˮƽ����ģʽ/ѡ������ģʽ
            �������������������ý���
            ��ת��������ˮƽ�������Σ�����ͣ����״̬����Ч��
            ����ͬʱ��ת����������ֱ�������Σ�������ͣ����״̬����Ч��
            
        ���ý��棺
            ��������������Ч
            ˫������������Ч
            ����������������������
            ��ת���������޸ĵ�ǰѡ��
            ����ͬʱ��ת���������л�ѡ��

    �����������
        ����ʱ�����䣺"500ms", "200ms", "100ms", "50ms", "20ms", "10ms","5ms", "2ms", "1ms", "500us", "200us", "100us"(100us�����Զ�����ģʽ�¿���)
        �����ѹ���䣺�趨��ѹ�������ޣ����30V��˳ʱ����ת����������ѹ��������Ϊ0������Զ����̣���ʱ����ת�����ֶ�����
        ����ֵ���趨������ѹֵ������λ�ñ�������Ļ����
        �����������¼�ͷ���ֱ���������ش������½��ش���
        ������ʽ��
            Auto(�Զ�����)  ������������ָʾ��ʼ�յ�����ͨ������������������ͣ�������ɹ�ʱ��������λ����Ļ���ģ�����ʧ�������޹��ɹ�������Ļ�����ʾFail��־
            Normal(��ͨ����)���ȴ�Ԥ��������ָʾ�������ʾԤ������������ʱ���������źţ�
                              �����ɹ�����ʾ���Σ��������ȴ��´δ�����
                              ������µĴ����ɹ������Զ���ʾ�µĲ��Σ���û���µĴ����ɹ�����Ļ���α��ֲ�һֱ�ȴ��´δ�����
                              ��ע��������ȷ�Ĵ���ֵ������һֱ���ڵȴ�����״̬����Ļ������ʾ����
            Single(���δ���)���ȴ�Ԥ��������ָʾ�������ʾԤ������������ʱ���������źţ�
                              �����ɹ�����ʾ���Σ���ֹͣ���������û�������������ʼ�´β���	
                              ��ע��������ȷ�Ĵ���ֵ������һֱ���ڵȴ�����״̬����Ļ������ʾ����
            
    ������״̬��ʾ��	
        Run�����ڲ���
        Stop��ֹͣ����
        Fail���Զ�ģʽ�£�����ֵ�������η�Χ���𴥷�ʧ��
        Auto���Զ�����
            
    ���ý���ѡ�
        PMode(��ͼģʽ)��
            Vector��������ʸ����ʾ
            Dots  �������Ե�����ʾ
        LSB(����ϵ��)����ѹϵ����100����������10k��2k�ĵ�����з�ѹ����ѹϵ��Ϊ(10+2/2)=6��LSB����ȡֵΪ6x100=600������ʵ�ʵ��辫�Ƚ���΢��	
        BRT��OLED����

    �������ã������������˳����ý���ʱ�����ý��������������в��������浽EEPROM

    2020-04-30����
    1. ���ӵ��δ�������ͨ�������ܣ����Զ��崥����������׽���α仯
    2. ���չ����Ҫ��ADC�����˿�����Ϊ��������
    2020-5-3����
    1. ���Ӳ���Ƶ����ʾ
    2. ʹ�ñ�����ˮƽ/��ֱ��������
    3. ʹ�ñ�������ֱ���Ų���
    4. �û����趨������ƽ
    2022-7-13����
    1. ��Ƭ���ͺ��޸�Ϊ "STC32G12K128"��Ĭ����Ƶ 35MHz
    2. ��"config_stc.h"�޸���ʾ��ʽΪSTC-ISP���Խӿ��������OLED����ʾ������Ӳ��OLED����ʾ
    3. �޸�EEPROM���ʳ���
    4. �޸İ������ܣ�P3.2�ڰ����̰�����ʱ���ᣬ����ֹͣ/�����ɼ���P3.3�ڰ����̰�����ʱ���ᣬ������ʾ/�˳����ò˵�
    5. ʹ�õ��Խӿڵ�����OLED��ʾ��ͨ������P3.2�ڰ�����������OLED-128*64��Ļ��ʾ
*/

#include "stc32g.h"
#include "global.h"
#include "delay.h"
#include "adc_stc.h"
#include "config_stc.h"
#include "settings.h"
#include "chart.h"
#include "ssd1306.h"
#include "stc.h"
#include "usb.h"

#define Timer1_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 1 �ж�Ƶ��, 1000��/��

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

#define VBAT_UPDATE_FREQ 40 //��ص�ѹ��Ϣ�ĸ���Ƶ�� VBAT_UPDATE_FREQ*25ms
#define VBAT_LSB 200        //��ص�ѹ������ѹϵ��*100
#define INT_0 0             //�ⲿ�ж�0��ţ���������ת
#define INT_1 2             //�ⲿ�ж�1��ţ�����������
#define TIMER_0 1           //��ʱ��0�ж����

#define KEY_TIMER 30        //�����������ʱ��(ms)

sbit KEY1 = P3 ^ 2;
sbit KEY2 = P3 ^ 3;

uint16 Key1_cnt;
uint16 Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key1_Short_Flag;
bit Key2_Short_Flag;

bit oled_init_flag;

bit PWM1_Flag;
uint16 PWM1_Duty;

void KeyScan(void);

void init()
{
    InSettings = 0;         //��ʼ����������
    OptionInSettings = 0;   //��ʼ�����ý����ѡ����
    OptionInChart = 0;      //��ʼ���������ѡ����
    WaveScroll = 0;         //���㲨�ι�����־
    OptionChanged = 0;      //���������޸ı�־
    ADCRunning = 1;         //��λ������־
    ADCInterrupt = 0;       //����ADC�жϱ�־
    ADCComplete = 0;        //����ADC��ɱ�־
    WaveUpdate = 0;         //���㲨�θ��±�־
    ClearWave = 0;          //����������α�־
    ClearDisplay = 1;       //��λ�����Ļ��־
    EC11PressAndRotate = 0; //�����������ת��־
    UpdateVbat = 0;         //���������ص�ѹ��־
    ADCComplete = 0;        //����ADC��ɲ�����־
    WaveFreq = 0;           //���㲨��Ƶ��
    TriPos = 50;            //��ʼ��������λ��
    TriPosOffset = 0;       //��ʼ��������ƫ����
    TriFail = 0;            //���㴥��ʧ�ܱ�־
    VMax = 0;               //���㲨�ε�ѹ���ֵ
    VMin = 0;               //���㲨�ε�ѹ��Сֵ
    WaveLengthSumNum = 0;   //���㲨�γ�����ʹ���
    WaveLengthSum = 0;      //���㲨�γ������

    /* ��ȡ�ڲ��ο���ѹֵ
       Get internal reference voltage */
    BGV = GetBGV();
}

void runWhenInSettrings()
{
    P_Ready = 0;
    ClearDisplay = 1;
    while (InSettings)
    {
        /* ���µ�ص�ѹ��Ϣ
           Update voltage infor of battery */
        if (UpdateVbat)
        {
            UpdateVbat = 0;
            VBat = GetVoltage(ADC_BAT, VBAT_LSB);
        }

        /* ������Ļ����
           Update OLED Brightness */
        if (OLED_BrightnessChanged)
        {
            OLED_BrightnessChanged = 0;
            
#if(VirtualDevice)
            OLED12864_SetContrast(OLED_Brightness * 10);    //����ֵ����Χ0��255��
#else
            OLED_Write_Command(0x81);
            OLED_Write_Command(OLED_Brightness * 10);
#endif
        }

        /* ���²���
           Update parameters */
        if (DisplayUpdate)
        {
            DisplayUpdate = 0;
            PlotSettings();
            OLED_Display();
        }
    }

    /* ��������
       Save Settings */
//    EA = 0;
    if (OptionChanged)
    {
        OptionChanged = 0;
        ClearDisplay = 0;
        /* �������ò���ʾ����״̬
           Save settings and display saving status*/
        PlotSaveStatus(Save_Options());
        OLED_Display();
        delay_ms(500);
    }

    /* �˳�settings��ˢ����Ļ
       Redraw display after exit settings */
    ClearDisplay = 1;
    PlotChart();
    OLED_Display();
//    EA = 1;
}

/*
    ADCRuning=1
    |	WaveUpdate=1
    |	|	ClearWave=1
    |	|	ClearWave=0
    |	WaveUpdate=0
    ADCRuning=0
        DisplayUpdate=1
            WaveUpdate=1
            |	ClearWave=1
            |	ClearWave=0
            WaveUpdate=0
*/
void runWhenADCInterrupt()
{
    ADCInterrupt = 0;
    P_Ready = 0;

    /* ADC��������
       ADC Sampling Running */
    if (ADCRunning)
    {
        if (WaveUpdate)
        {
            WaveUpdate = 0;
            ClearDisplay = 1;

            /* ��ղ��λ���
               Clear waveform in buffer*/
            if (ClearWave)
            {
                ClearWave = 0;
                memset(ADCbuf, 0x00, SAMPLE_NUM * 2);
                WaveFreq = 0;
                TriFail = 0;
                VMax = 0;
                VMin = 0;
                PlotChart();
            }
            /* ��ʾ�����еĲ���
               Display waveform in buffer*/
            else
            {
                AnalyseData();
                PlotChart();
                PlotWave();
            }
        }
        else
        {
            ClearDisplay = 0;
            PlotChart();
        }
        OLED_Display();
    }

    /* ADC����ֹͣ
       ADC Sampling Stopped */
    else
    {
        while (!ADCRunning && !InSettings)
        {
            if (DisplayUpdate)
            {
                DisplayUpdate = 0;
                if (WaveUpdate)
                {
                    WaveUpdate = 0;
                    /* ������������ɲ����Ĳ�������
                       Analyse completed sampling data in buffer */
                    AnalyseData();
                    ClearDisplay = 1;

                    /* �����Ļ�����Ʋ���
                       Clear display and draw waveform */
                    if (ScaleH == ScaleH_tmp)
                    {
                        /* �����������ޣ���֧�ֺ������Ų��Σ�ֻ��ʱ����������ɲ�����ʱ��������ͬ�Ż��Ʋ���
                           Since the sampling points limitation, scaling waveform along horizontal is not support.
                           Show waveform only when time scale is same as that when sampling completed */
                        PlotChart();
                        PlotWave();
                    }

                    /* �����Ļ�������Ʋ���
                       Clear display and no waveform */
                    else
                    {
                        WaveFreq = 0; //����WaveFreq�����ƽ���ʱ��Ƶ��λ�û���ʾ****Hz
                        PlotChart();
                    }
                }
                /* ֻ������Ļ�ϵĲ��������β���
                   Update parameters on display only, keep waveform */
                else
                {
                    ClearDisplay = 0;
                    PlotChart();
                }

                OLED_Display();
            }
        }
    }
}

void runWhenADCComplete()
{
    /*  ADC������� - ���δ���
        ADC Sampling Complete - Single Trigger Mode */
    if (TriMode == 2)
    {
        ADCInterrupt = 1;
        DisplayUpdate = 0;
        WaveUpdate = 0;
        ADCRunning = 0; //����ADC���б�־��ֹͣ����
        AnalyseData();
        ClearDisplay = 1;
        PlotChart();
        PlotWave();
        OLED_Display();
    }

    /*  ADC������� - �Զ���������ͨ����
        ADC Sampling Complete - Auto or Normal Trigger Mode */
    else
    {
        ClearDisplay = 1; //��λ�����Ļ��־
        AnalyseData();    //������������
        PlotChart();      //����������
        PlotWave();       //���Ʋ���
        OLED_Display();
    }
}

void Timer0Init(void)		//25����@1MHz
{
    TM0PS = 34;     //35��Ƶ
	AUXR |= 0x80;	//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;	//���ö�ʱ��ģʽ
	TL0 = 0x58;		//���ö�ʱ��ʼֵ
	TH0 = 0x9E;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
//	TR0 = 1;		//��ʱ��0��ʼ��ʱ
    ET0 = 1;        //����ʱ��0�ж�
}

void Timer1Init(void)		//1����
{
	AUXR |= 0x40;	//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;	//���ö�ʱ��ģʽ
	TH1 = (uint8)(Timer1_Reload / 256);	//���ö�ʱ��ʼֵ
	TL1 = (uint8)(Timer1_Reload % 256);	//���ö�ʱ��ʼֵ
	TF1 = 0;		//���TF1��־
    ET1 = 1;		//�����ж�
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
}

void PWMInit(void)
{
    PWMA_CCER1 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCMR1 = 0x68; //ͨ��ģʽ���ã�����CCRԤװ�ع���
    PWMA_CCER1 = 0x05; //����ͨ�����ʹ�ܺͼ���

    PWMA_ARRH = 0x00;  //��������ʱ��
    PWMA_ARRL = 0x0f;
    PWMA_CCR1H = 0;
    PWMA_CCR1L = 0;    //����ռ�ձ�ʱ��

    PWMA_ENO = 0x01;   //PWM1Pʹ�����
    PWMA_PS = 0x00;    //�߼� PWM ͨ�������ѡ��λ

    PWMA_BKR = 0x80;   //ʹ�������
    PWMA_CR1 |= 0x81;  //ARR��Ԥװ�ػ��������壬��ʼ��ʱ
}

void UpdatePwm(void)
{
    PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR1L = (u8)(PWM1_Duty);
}

/* �������
   Program Entry */
void main()
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�
    
    /* ��ADC�˿�P0.4��P0.6����Ϊ����
       Set ADC port P0.4, P0.6 as high-impedance */
//    P0M1 |= 0x50;
//    P0M0 &= ~0x50;

    P0M1 = 0x00;
    P0M0 = 0x03;

    /* ���ö˿�P1Ϊ׼˫��
       Set port P1 as quasi bidirectional */
//    P1M1 &= ~0x02;
//    P1M0 |= 0x02;
    P1M1 = 0x10;
    P1M0 = 0x00;

    /* ����OLED�˿�P2.3��P2.4��P2.5��P2.6��P2.7Ϊ׼˫��
       Set OLED port P2.3, P2.4, P2.5, P2.6 as quasi bidirectional */
//    P2M1 &= ~0xF8;
//    P2M0 &= ~0xF8;
    P2M1 = 0x00;
    P2M0 = 0x00;

    /* ���ñ������˿�P3.2��P3.3��P3.4Ϊ׼˫��
       Set Encoder port P3.2, P3.3, P3.4 as quasi bidirectional */
//    P3M1 &= ~0x1C;
//    P3M0 &= ~0x1C;
    P3M1 = 0x00;
    P3M0 = 0x00;

    /* ����ָʾ�ƶ˿�P4Ϊ׼˫��
       Set indicator port P4 as quasi bidirectional */
    P4M1 = 0x00;
    P4M0 = 0x00;

    P7M1 = 0x00;
    P7M0 = 0x00;

    PWM1_Flag = 0;
    PWM1_Duty = 0;
    
    ADC_POWER = 1;       //ʹ�� ADC ģ��
    PWMInit();
    Timer0Init();
    Timer1Init();

    IAP_TPS = 35; //����EEPROM����ʱ�� 35MHz/1000000
    Read_Options();

    init();
    OLED_SetFontWidth(6);
    PlotChart();

#if(VirtualDevice)
    usb_init();

    /* �����ж�
       Enable global interrupt */
    EUSB = 1;   //IE2��ص��ж�λ����ʹ�ܺ���Ҫ��������EUSB
    EA = 1;
#else
    HAL_SPI_Init();
    /* �����ж�
       Enable global interrupt */
    EA = 1;
    OLED_Init();
    OLED_Display();
    oled_init_flag = 1;
#endif

    while (1)
    {
#if(VirtualDevice)
        if(DeviceState != DEVSTATE_CONFIGURED)  //�ȴ�USB�������
            continue;

        if (bUsbOutReady)
        {
            usb_OUT_done();
        }
#endif
        
        if(oled_init_flag == 0)
            continue;

        P_Ready = 0;

        /* ��������
           Sampling*/
        GetWaveData();
        
        /* �������ý���
           Enter Settings Interface */
        if (InSettings)
        {
            runWhenInSettrings();
        }

        /* ADC�����ж�
           ADC Sampling Interrupt */
        else if (ADCInterrupt)
        {
            runWhenADCInterrupt();
        }

        /* ADC�������
           ADC Sampling Complete */
        else
        {
            runWhenADCComplete();
        }
    }
}

/* Ϊ����Vbat��ʱ
   Timer for updating Vbat */
void TIMER0_interrupt(void) interrupt TIMER_0
{
    static uint8 n;

    if (++n >= VBAT_UPDATE_FREQ)
    {
        n = 0;
        UpdateVbat = 1;
        DisplayUpdate = 1;
    }
}

void TIMER1_interrupt(void) interrupt 3
{
    KeyScan();

//    if(!PWM1_Flag)
//    {
//        PWM1_Duty++;
//        if(PWM1_Duty >= 8) PWM1_Flag = 1;
//    }
//    else
//    {
//        PWM1_Duty--;
//        if(PWM1_Duty <= 0) PWM1_Flag = 0;
//    }
//    UpdatePwm();
}

//========================================================================
// ����: void KeyScan(void)
// ����: ����ɨ�躯����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void KeyScan(void)
{
	//�̰�����
	if(!KEY1)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt >= 1000)		//����1s
			{
				Key1_Short_Flag = 0;	//����̰���־
//				Key1_Long_Flag = 1;		//���ó�����־
				Key1_Flag = 1;				//���ð���״̬����ֹ�ظ�����
//				Key1_Long_Function = 1;

                /* ����OLED��ʼ�� */
                if(oled_init_flag == 0)
                {
                    oled_init_flag = 1;
                    OLED_Init();
                    OLED_Display();
                }
                /* �������棬�л�Stop/Run״̬ - Switch Run/Stop in main interface    */
                else if (!InSettings)
                {
                    ADCRunning = ~ADCRunning;
                    if (ADCRunning)
                    {
                        WaveUpdate = 1;
                        ClearWave = 1;
                    }
                    else
                    {
                        DisplayUpdate = 1;
                        WaveUpdate = 1;
                    }
                    ADCInterrupt = 1;
                }
            }
			else if(Key1_cnt >= KEY_TIMER)	//30ms����
			{
				Key1_Short_Flag = 1;		//���ö̰���־
			}
		}
	}
	else
	{
		if(Key1_Short_Flag)			//�ж��Ƿ�̰�
		{
			Key1_Short_Flag = 0;	//����̰���־
            Change_Val(1);      /* ��ת  Clockwise */
        }
		Key1_cnt = 0;
		Key1_Flag = 0;
	}

	//�̰�����
	if(!KEY2)
	{
		if(!Key2_Flag)
		{
			Key2_cnt++;
			if(Key2_cnt >= 1000)		//����1s
			{
				Key2_Short_Flag = 0;	//����̰���־
//				Key2_Long_Flag = 1;		//���ó�����־
				Key2_Flag = 1;				//���ð���״̬����ֹ�ظ�����
//				Key2_Long_Function = 1;

            /* �л�����������ý��� - Switch main interface and settings interface */
            InSettings = ~InSettings;

            /*  �������ý���
                Enter Settings  */
            if (InSettings)
            {
                DisplayUpdate = 1;
                UpdateVbat = 1;
                TF0 = 0; //���㶨ʱ��0�����־
                TR0 = 1; //��ʱ��0��ʼ��ʱ����ʼ��ص�ѹ��Ϣ���¼�ʱ
            }

            /*  �ص�������
                Retrurn to main interface   */
            else
            {
                TR0 = 0; //���㶨ʱ��0�����־
                TF0 = 0; //��ʱ��0ֹͣ��ʱ��ֹͣ��ص�ѹ��Ϣ���¼�ʱ
                WaveFreq = 0;
                TriFail = 0;
                VMax = 0;
                VMin = 0;
                DisplayUpdate = 1;
                WaveUpdate = 1;
                ClearWave = 0;
            }
            ADCInterrupt = 1;
            }
			else if(Key2_cnt >= KEY_TIMER)	//30ms����
			{
				Key2_Short_Flag = 1;		//���ö̰���־
			}
		}
	}
	else
	{
		if(Key2_Short_Flag)			//�ж��Ƿ�̰�
		{
			Key2_Short_Flag = 0;	//����̰���־
            Change_Val(0);          /* ��ת  Anticlockwise */
        }
		Key2_cnt = 0;
		Key2_Flag = 0;	//�����ͷ�
	}
}
