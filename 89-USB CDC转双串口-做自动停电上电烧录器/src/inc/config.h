#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FOSC                    24000000UL

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

#define ISPPWRCTRL                      //ISP����ʱSVCC��Դ�Զ����ƿ���
                                        //ʹ�ܴ˹���ʱ,�����STC-ISP���ز���SVCC�����Զ�ͣ�����ϵ�
                                        //������Ҫ�˹���ֻ��Ҫע�ʹ��м���

#endif
