#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "rng.h"
#include "skiplist.h"
#define MAX_VALUE 0x7FFFFFFF

typedef struct snode
{
	int value;
	int nblevels;
	struct snode **backward;
	struct snode **forward;
}snode;

typedef struct s_SkipList
{
	unsigned int size;
	snode *sentinel;
    RNG r;
}s_SkipList;

snode* skiplist_node_create(int value, int nblevels);
void skiplist_node_delete(snode *s);


SkipList skiplist_create(int nblevels)
{
		int i;
		SkipList list=(SkipList)malloc(sizeof(struct s_SkipList));
        assert(list != NULL);
        snode *sentinel=skiplist_node_create(MAX_VALUE,nblevels);
		list->sentinel=sentinel;
		for (i = 0; i <= nblevels-1; i++)
		{
			sentinel->forward[i] = list->sentinel;
			sentinel->backward[i] = list->sentinel;
		}
		list->size=0;
        list->r=rng_initialize(0);
		return list;
}

snode* skiplist_node_create(int value, int nblevels)
{
    snode *new_node=(snode *)malloc(sizeof(snode));
    assert(new_node != NULL);
    new_node->value=value;
    new_node->nblevels=nblevels;
    new_node->forward  = (snode **)malloc(sizeof(snode*) * (nblevels));
    assert(new_node->forward!=NULL);
    new_node->backward = (snode **)malloc(sizeof(snode*) * (nblevels));
    assert(new_node->backward!=NULL);
   return new_node;
}

void skiplist_node_delete(snode *s)
{
    if (s)
    {
        free(s->forward);
        free(s->backward);
        free(s);    
    }
}

void skiplist_delete(SkipList d)
{
	snode *current_node= d->sentinel->forward[0];
	while(current_node != d->sentinel)
	{
		snode *next_node= current_node->forward[0];
		skiplist_node_delete(current_node);
		current_node=next_node;
	}
	skiplist_node_delete(d->sentinel);
	free(d);
}

unsigned int skiplist_size(SkipList d)
{
	return d->size;
}

int skiplist_ith(SkipList d, unsigned int i)
{
	unsigned int j=0;
	snode *ith=d->sentinel->forward[0];
	while (j++ != i && ith != d->sentinel)
	{
		ith=ith->forward[0];
	}
	return ith->value;
}

SkipList skiplist_insert(SkipList d, int value)
{
    snode *update[d->sentinel->nblevels];
    snode *x = d->sentinel;
    int i, level;
    for (i = d->sentinel->nblevels-1; i >= 0; i--) {
        while (x->forward[i]->value < value)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[0];
    if (value != x->value)
    {
        level=rng_get_value(&(d->r),d->sentinel->nblevels-1);
        level++;
        if (level > d->sentinel->nblevels) {
            for (i = d->sentinel->nblevels; i <= level; i++) {
                update[i] = d->sentinel;
            }
            d->sentinel->nblevels = level;
        }
        x=skiplist_node_create(value, level );
        for (i = 0; i <= level-1; i++) {
            x->forward[i] = update[i]->forward[i];
            x->backward[i] = update[i];
            update[i]->forward[i]->backward[i]=x;
            update[i]->forward[i] = x;
        }
        d->size++;
    }
    return d;
}

SkipList skiplist_remove(SkipList d, int value)
{
	int i;
    snode *update[d->sentinel->nblevels];
    snode *x = d->sentinel;
    for (i = d->sentinel->nblevels-1; i >= 0; i--) {
        while (x->forward[i]->value < value)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[0];
    if (x->value == value) {
        for (i = 0; i <= d->sentinel->nblevels-1; i++) {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[i] = x->forward[i];
            x->forward[i]->backward[i] = update[i];
        }
        skiplist_node_delete(x);
        while (d->sentinel->nblevels > 1 && d->sentinel->forward[d->sentinel->nblevels] == d->sentinel)
            d->sentinel->nblevels--;

        d->size--;
    }
    return d;
}

bool skiplist_search(SkipList d, int value, unsigned int *nb_operations)
{   *nb_operations=0;
	snode *current_node=d->sentinel;
	int i;
	for (i = d->sentinel->nblevels-1; i >= 0 ; i--)
	{
			while (current_node->forward[i]->value < value)
			{
            	current_node = current_node->forward[i];
            	(*nb_operations)++;
            }
	}
	return (current_node->forward[0]->value == value);
}

void skiplist_map(SkipList d, ScanOperator f, void *user_data)
{
	snode *current_node=d->sentinel->forward[0];
	while(current_node != d->sentinel)
	{
		f(current_node->value, user_data);
		current_node=current_node->forward[0];
	}
}

void skiplist_dump(SkipList list, int order)
{
    printf("Skiplist (%d)\n",list->size);
    snode *x = list->sentinel;
    if (order == 1)
    {
        while (x && x->forward[0] != list->sentinel) { 
        printf("%d ",  x->forward[0]->value);
        x = x->forward[0];
        }
    }
    else
    {
        while (x && x->backward[0] != list->sentinel) { 
        printf("%d ",  x->backward[0]->value);
        x = x->backward[0];
        }
    }
    printf("\n");
}

void skiplist_view(SkipList list)
{
    snode *x = list->sentinel;
    int i;
    for (i = list->sentinel->nblevels-1; i >= 0; i--)
    {
        while (x && x->forward[i] != list->sentinel)
        {
            printf(" [%d] -> ",x->forward[i]->value);
            x = x->forward[i];
        }
        x=list->sentinel;
        printf("NIL\n");
    }
    printf("La taille de la liste est %d\n",list->size );
    printf("Le niveau de la liste est %d\n",list->sentinel->nblevels );
}

typedef struct s_SkipListIterator
{
    SkipList d;
    snode* current;
    unsigned char w;
}s_SkipListIterator;

SkipListIterator skiplist_iterator_create(SkipList d, unsigned char w)
{
    SkipListIterator it=(SkipListIterator)malloc(sizeof(struct s_SkipListIterator));
    it->d = d;
    it->w = w;
    it->current = NULL;
    return it;
}

void skiplist_iterator_delete(SkipListIterator it)
{
    skiplist_delete(it->d);
    free(it);
}

SkipListIterator skiplist_iterator_begin(SkipListIterator it)
{
    if (it->w == FORWARD_ITERATOR)
    {
        it->current = it->d->sentinel->forward[0];
    }
    else if (it->w == BACKWARD_ITERATOR)
    {
        it->current = it->d->sentinel->backward[0];
    }
    return it;
}

bool skiplist_iterator_end(SkipListIterator it)
{
    return (it->current == it->d->sentinel);
}

SkipListIterator skiplist_iterator_next(SkipListIterator it)
{
    if (it->w == FORWARD_ITERATOR)
    {
        it->current = it->current->forward[0];
    }
    else if (it->w == BACKWARD_ITERATOR)
    {
        it->current = it->current->backward[0];
    }
    return it;
}

int skiplist_iterator_value(SkipListIterator it)
{
    return it->current->value;
}