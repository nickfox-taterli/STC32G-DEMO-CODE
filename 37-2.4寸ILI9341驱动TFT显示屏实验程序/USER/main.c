/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 液晶屏驱动程序参考wiki技术网站提供的开源源码，仅供学习使用          */
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

//=========================================电源接线================================================//
//VDD接DC 3.3V电源
//GND接地
//=======================================液晶屏数据线接线==========================================//
//本模块数据总线类型为8位并口
//液晶屏模块               STC32G
// LCD_D0~D7     接       P60~P67        //液晶屏8位数据线
//=======================================液晶屏控制线接线==========================================//
//液晶屏模块               STC32G
// LCD_RS        接        P45           //数据/命令选择控制信号
// LCD_RD        接        P44           //读控制信号
// LCD_RST       接        P43           //复位信号
// LCD_WR        接        P42           //写控制信号
// LCD_CS        接        P34           //片选控制信号
//================================================================================================//
// 本实验测试使用的2.4寸320*240的并口彩屏，是在淘宝店 https://kcer001.taobao.com/ 提供的样品彩屏上测试通过的
//**************************************************************************************************/
#include "sys.h"
#include "lcd.h"
#include "gui.h"
#include "test.h"

//主函数
void main(void)
{ 
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

	//液晶屏初始化
	LCD_Init();

	//循环进行各项测试	
	while(1)
	{	
		main_test(); 		//测试主界面
		Test_Read();     //读ID和颜色值测试
		Test_Color();  		//简单刷屏填充测试
		Test_FillRec();		//GUI矩形绘图测试
		Test_Circle(); 		//GUI画圆测试
		Test_Triangle();   //GUI三角形填充测试
		English_Font_test();//英文字体示例测试
		Chinese_Font_test();//中文字体示例测试
		Pic_test();			//图片显示示例测试
		Rotate_Test();  //屏幕旋转测试
	}   
}
