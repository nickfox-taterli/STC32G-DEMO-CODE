#ifndef _ALG_LINEARLIST_H_
#define _ALG_LINEARLIST_H_

#define uchar unsigned char
#define uint unsigned int

/************************************************************************/
/* ���Ա�ṹ����                                                       */
/************************************************************************/
#define LArray_FALSE 0
#define LArray_TRUE  1

//ջ�Ĵ�С����
#define STACK_SIZE 30

typedef unsigned char LArray_BOOL;

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

#endif
