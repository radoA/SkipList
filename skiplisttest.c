#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skiplist.h"
#define ERREUR_OUVERTURE_FICHIER 1
#define MAX_VALUE 0x7FFFFFFF

void usage(const char *command) {
	printf("usage : %s -id num\n", command);
	printf("where id is :\n");
	printf("\tc : construct and print the skiplist with data read from file test_files/construct_num.txt\n");
	printf("\ts : construct the skiplist with data read from file test_files/construct_num.txt and search elements from file test_files/search_num..txt\n\t\tPrint statistics about the searches.\n");
	printf("\ti : construct the skiplist with data read from file test_files/construct_num.txt and search, using an iterator, elements read from file test_files/search_num.txt\n\t\tPrint statistics about the searches.\n");
	printf("\tr : construct the skiplist with data read from file test_files/construct_num.txt, remove values read from file test_files/remove_num.txt and print the list in reverse order\n");
	printf("where num is the file number for input\n");
}

SkipList construction(int num){
	int value;
	int nblevels;
	char nom_fichier[80];
	SkipList d;
	sprintf(nom_fichier,"test_files/construct_%d.txt",num);
	FILE *f = fopen(nom_fichier,"r+");
	if (f==NULL)
	{
		fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nom_fichier);
		exit(ERREUR_OUVERTURE_FICHIER);
	}
	fscanf(f,"%d",&nblevels);
	d=skiplist_create(nblevels);
	fscanf(f,"%d\n",&value);
	while(!feof(f))
	{
		fscanf(f,"%d\n",&value);
		d=skiplist_insert(d, value);
	}
	fclose(f);
	return d;
}

void test_construction(int num){
	SkipList d=construction(num);
	skiplist_dump(d,1 );
	skiplist_delete(d);
}

void affiche_infos(int size, int nb_value_to_find, int nb_found, int nb_not_found, int min, int max, int mean)
{
	printf("Statistics : \n");
	printf("\tSize of the list : %d\n", size );
	printf("Search %d values :\n", nb_value_to_find);
	printf("\tFound %d\n", nb_found);
	printf("\tNot found %d\n", nb_not_found );
	printf("\tMin number of operations : %d\n", min );
	printf("\tMax number of operations : %d\n", max);
	printf("\tMean number of operations : %d\n", mean);
}

void test_search(int num){
	SkipList d=construction(num);
	int nb_value_to_find, value, nb_found=0, nb_not_found=0;
	unsigned int nb_operations, min=MAX_VALUE, max= 0, tot_nb_operations=1;
	char nom_fichier[80];
	sprintf(nom_fichier,"test_files/search_%d.txt",num);
	FILE *f = fopen(nom_fichier,"r+");
	if (f==NULL)
	{
		fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nom_fichier);
		exit(ERREUR_OUVERTURE_FICHIER);
	}
	fscanf(f,"%d\n",&nb_value_to_find);
	while(!feof(f))
	{
		fscanf(f,"%d\n",&value);
		if (skiplist_search(d, value, &nb_operations))
		{
			printf("%d -> true\n", value);
			nb_found++;
		}
		else
		{
			printf("%d -> false\n", value);
			nb_not_found++;
		}
		tot_nb_operations+=nb_operations+1;
		if (nb_operations < min && nb_operations != 0)
		{	
			min = nb_operations;
		}
		if (nb_operations > max)
		{
			max =nb_operations+1;
		}
	}
	fclose(f);
	affiche_infos(skiplist_size(d), nb_value_to_find, nb_found, nb_not_found, min, max, tot_nb_operations/nb_value_to_find );
	skiplist_delete(d);
}

bool skiplist_iterator_search(SkipListIterator it, int value, unsigned int *nb_operations)
{
    *nb_operations=0;
    for (it = skiplist_iterator_begin(it); !skiplist_iterator_end(it); it = skiplist_iterator_next(it))
    {
        (*nb_operations)++;
        if (skiplist_iterator_value(it) == value)
        {
            return true;
        }
    }
    return false;
}


void test_search_iterator(int num){
	SkipList d=construction(num);
	SkipListIterator it =skiplist_iterator_create(d, FORWARD_ITERATOR);
	int nb_value_to_find, value, nb_found=0, nb_not_found=0;
	unsigned int nb_operations, min=MAX_VALUE, max= 0, tot_nb_operations=1;
	char nom_fichier[80];
	sprintf(nom_fichier,"test_files/search_%d.txt",num);
	FILE *f = fopen(nom_fichier,"r+");
	if (f==NULL)
	{
		fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nom_fichier);
		exit(ERREUR_OUVERTURE_FICHIER);
	}
	fscanf(f,"%d\n",&nb_value_to_find);
	while(!feof(f))
	{
		fscanf(f,"%d\n",&value);
		if (skiplist_iterator_search(it, value, &nb_operations))
		{
			printf("%d -> true\n", value);
			nb_found++;
		}
		else
		{
			printf("%d -> false\n", value);
			nb_not_found++;
		}
		tot_nb_operations+=nb_operations;
		if (nb_operations <= min && nb_operations != 0)
		{	
			min = nb_operations;
		}
		if (nb_operations > max)
		{
			max =nb_operations;
		}
	}
	fclose(f);
	affiche_infos(skiplist_size(d), nb_value_to_find, nb_found, nb_not_found, min, max, tot_nb_operations/nb_value_to_find );
	skiplist_iterator_delete(it);
}

void test_remove(int num){
	SkipList d=construction(num);
	char nom_fichier[80];
	int nb_value_to_remove, value;
	sprintf(nom_fichier,"test_files/remove_%d.txt",num);
	FILE *f = fopen(nom_fichier,"r+");
	if (f==NULL)
	{
		fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", nom_fichier);
		exit(ERREUR_OUVERTURE_FICHIER);
	}
	fscanf(f,"%d\n",&nb_value_to_remove);
	while(!feof(f))
	{
		fscanf(f,"%d\n",&value);
		d=skiplist_remove(d, value);
	}
	fclose(f);
	skiplist_dump(d,0);
	skiplist_delete(d);
}

void generate(int nbvalues);


int main(int argc, const char *argv[]){
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}
	switch (argv[1][1]) {
		case 'c' :
			test_construction(atoi(argv[2]));
			break;
		case 's' :
			test_search(atoi(argv[2]));
			break;
		case 'i' :
			test_search_iterator(atoi(argv[2]));
			break;
		case 'r' :
			test_remove(atoi(argv[2]));
			break;
		case 'g' :
			generate(atoi(argv[2]));
			break;
		default :
			usage(argv[0]);
			return 1;
	}
	return 0;
}



void generate(int nbvalues){
	FILE *output;
	int depth;
	int maxvalue;
	output = fopen("construct.txt", "w");
	srand(nbvalues);
	depth = rand()%16;
	maxvalue = rand()%10 * nbvalues;
	fprintf(output, "%d\n%d\n", depth, nbvalues);
	for (int i=0; i< nbvalues; ++i) {
		fprintf(output, "%d\n", rand()%maxvalue);
	}
	fclose(output);
	output = fopen("search.txt", "w");
	srand(rand());
	nbvalues *= depth/4;
	fprintf(output, "%d\n", nbvalues);
	for (int i=0; i< nbvalues; ++i) {
		fprintf(output, "%d\n", rand()%maxvalue);
	}
	fclose(output);
}

