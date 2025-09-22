#ifndef __DoubleCircularLinkedList_H__
#define __DoubleCircularLinkedList_H__


//数据结点中数据元素的类型
typedef char * dElemType;

//数据结点的类型 
typedef struct biNode
{
	dElemType data; //数据域 
	struct biNode * next; 
	struct biNode * prev;
}BiNode;

//头结点类型
typedef struct biHead
{
	BiNode * first;
	BiNode * last;
	int num;
}BiHead;

BiHead * Create_Head(void);
BiHead * Create_dc_list(void);
void Print_CirList(BiHead * h);
void Delete_X(BiHead * h, dElemType X);
void Destroy_CirList(BiHead ** h);

#endif 




