
#include <stdio.h>
#include <stdlib.h>

#include "DoubleCircularLinkedList.h"

/*
	Create_Head : 
		创建双向循环链表的头结点
	返回值: 返回头结点的指针
*/
BiHead * Create_Head(void)
{
	BiHead * h = malloc(sizeof(BiHead));
	h->first = NULL;
	h->last = NULL;
	h->num = 0;
	return h;
}


