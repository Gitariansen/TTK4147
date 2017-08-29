#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create()
{
	list_t newList = malloc(sizeof(struct list));
	newList->head = malloc(sizeof(struct node));
	newList->tail = malloc(sizeof(struct node));
	newList->length = 0;
	
	return newList;
}

void list_delete(list_t list)
{
	free(list);
}

void list_insert(list_t list, int index, int data)
{
	struct node* currentNode = list->head->next;
	int i;
	for (i = 0; i < index; i++)
	{
		currentNode = currentNode->next;
	}
	struct node* newNode = malloc(sizeof(struct node));
	newNode->data = data;
	newNode->next = currentNode;
	newNode->prev = currentNode->prev;
	newNode->next->prev = newNode;

	newNode->prev->next = newNode;
	list->length++;
}

void list_append(list_t list, int data)
{

	struct node* currentNode = list->head;
	while(currentNode->next != NULL)
	{
		currentNode = currentNode->next;
	}

	currentNode->next = malloc(sizeof(struct node));
	currentNode->next->data = data;
	currentNode->next->next = NULL;
	currentNode->next->prev = currentNode;
	list->length++;
	
}

void list_print(list_t list)
{

	struct node* currentNode = list->head->next;
	int i;
	for (i =0; i < list->length; i++)
	{
		printf("%i ", currentNode->data);
		currentNode = currentNode->next;
	}
	printf("\n");
}

long list_sum(list_t list)
{
	struct node* currentNode = list->head->next;
	int i;
	int sum = 0;
	for (i =0; i <list->length; i++)
	{
		sum += currentNode->data;
		currentNode = currentNode->next;
	}
	return sum;
}

int list_get(list_t list, int index)
{
	struct node* currentNode = list->head->next;
	int i;
	for (i = 0; i < index; i++)
	{
		currentNode = currentNode->next;
	}
	return currentNode->data;
}

int list_extract(list_t list, int index)
{
	struct node* currentNode = list->head->next;
	int i;
	for (i = 0; i < index; i++)
	{
		currentNode = currentNode->next;
	}
	
	if(currentNode->next == NULL)
	{
		int retVal = currentNode->data;
		free(currentNode);
		list->length--;
		return retVal;
	}
	currentNode->prev->next = currentNode->next;
	currentNode->next->prev = currentNode->prev;
	int retVal = currentNode->data;
	free(currentNode);
	list->length--;
	return retVal;
}
