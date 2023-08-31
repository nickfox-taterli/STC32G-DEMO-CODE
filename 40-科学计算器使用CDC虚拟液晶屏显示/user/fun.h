#ifndef __FUN_H
#define __FUN_H

#include "middle.h"
/************************************************************************/
/* ���Ա�ṹ����                                                       */
/************************************************************************/
#define LArray_FALSE 0
#define LArray_TRUE  1

extern double x,y,z;

//ջ�Ĵ�С����
#define STACK_SIZE 30
extern double g_ans;
typedef unsigned char LArray_BOOL;

extern uchar g_chCalcStatus;

/*Ԫ������*/
typedef enum
{
	NUMBER,						//����Ԫ��Ϊ������
	OPERATOR,					//����Ԫ��Ϊ���
	CONST_NUM,                  //����Ϊ��������
	FUNCTION,                   //Ϊ����
	INVALID_TYPE				//����Ԫ����Ч
}TYPE_FLAG;

/*�����������Ԫ��*/
typedef union
{
	double	dbNum;				//�������������
	char	cOperator;			//�����������
}TYPE;

/*���Ա�Ԫ��*/
typedef struct
{
	TYPE_FLAG	elemType;		//����Ԫ������
	TYPE		elemValue;		//����Ԫ��ʵ��ֵ�����
	double		dbComplex;		//�洢Ԫ���鲿ֵ
}LArrayElem;

/*���Ա�*/
typedef struct
{
	LArrayElem	*pElem;			//����Ԫ�ش洢����
	uchar		nLength;		//��ǰ����Ԫ�س���
	uchar		nTotalSize;		//��ǰ���Ա���
}LArray;



typedef enum
{
	LEVEL_BIGGER,		//���ȼ���
	LEVEL_SMALLER,		//���ȼ���
	LEVEL_SAME,			//���ȼ����
	LEVEL_INVALID		//������Ϸ�
}LEVEL_TYPE;

typedef enum
{
	E_FLAG,             //��Ȼ������־
	PI_FLAG,            //�б�־
	X_FLAG,
	Y_FLAG,
	Z_FLAG,
	ANS_FLAG,
	NO_CONST            //�ǳ���
}CONST_FLAG;

typedef enum
{
	LN_FLAG,
	LOG_FLAG,
	SIN_FLAG,
	COS_FLAG,
	TAN_FLAG,
	ABS_FLAG,
	SINH_FLAG,
	COSH_FLAG,
	TANH_FLAG,
	ASIN_FLAG,
	ACOS_FLAG,
	ATAN_FLAG,
	FUNC_INVALID		//�������Ϸ�
}FUNC_FLAG;


typedef enum
{
	CALC_NORMAL,		//��ͨ����
	CALC_COMPLEX,		//�������㣨֧�ּӼ��˳�����֧�ֺ�����
}CALC_STATUS;

uchar alg_calc_expression(char *pExpression, double *pRealResult, double *pImaginaryResult);

#define OPERATOR_COMPLEX(operator,eElemX,eElemY,eResult) complex_##operator(eElemX, eElemY, eResult)

double complex_mod(LArrayElem *eElem1);

/************************************************************************/
/* �������Ա�������                                                           */
/************************************************************************/
LArray* ll_arry_create(const uchar nSelect);
void ll_arry_clr(LArray *pArray );

LArray_BOOL ll_arry_is_empty(LArray *pArray);
LArray_BOOL ll_arry_is_full(LArray *pArray);
uchar ll_arry_get_len(LArray *pArray);


/************************************************************************/
/* ջ������������															*/
/************************************************************************/
LArray_BOOL stack_get_top(LArray *pArray, LArrayElem *pElem);
LArray_BOOL stack_push(LArray *pArray, const LArrayElem elem);
LArray_BOOL stack_pop(LArray *pArray, LArrayElem *pElem);

void complex_plus(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult); 	//�����ӷ�����������LArrayElem�ṹ���͵�ָ��
void complex_subtract(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult); //������������������LArrayElem�ṹ���͵�ָ��
void complex_multiply(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//�����˷�����������LArrayElem�ṹ���͵�ָ��
void complex_divide(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//������������������LArrayElem�ṹ���͵�ָ��
uchar complex_power(LArrayElem *eElem1 ,LArrayElem *eElem2, LArrayElem *eResult);	//�����˷�����������LArrayElem�ṹ���͵�ָ��

double alg_atof(char *s);
#endif


