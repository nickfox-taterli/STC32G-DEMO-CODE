#ifndef __CONFIG_H__
#define __CONFIG_H__

extern unsigned long MAIN_Fosc;

#define Dynamic_Frequency         //���ݲ����ʶ�̬������Ƶ���ƿ���
                                  //ʹ�ܺ�ɸ��ݴ��ڲ����ʶ�̬������Ƶ����ߴ���ͨ�ž���
                                  //����һ��ͨ�������ʵ���������Ƶ�仯����һ��ͨ���Ĳ����ʼ���ͻ���Ӱ��
                                  //����Ҫ�˹��ܿ������������


#define EP0_SIZE                64
#define EP1IN_SIZE              64
#define EP2IN_SIZE              64
#define EP3IN_SIZE              64
#define EP4IN_SIZE              64
#define EP5IN_SIZE              64
#define EP1OUT_SIZE             64
#define EP2OUT_SIZE             64
#define EP3OUT_SIZE             64
#define EP4OUT_SIZE             64
#define EP5OUT_SIZE             64

#define EN_EP2IN                        //����1������IN�˵�
#define EN_EP3IN                        //����2�Ŀ���IN�˵�
#define EN_EP4IN                        //����1������IN�˵�
#define EN_EP5IN                        //����2�Ŀ���IN�˵�
#define EN_EP4OUT                       //����1������OUT�˵�
#define EN_EP5OUT                       //����2������OUT�˵�

#endif
