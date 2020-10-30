/** \file driver.c
 * A simple test for the idmap module.
 */

#include <stdio.h>
#include<stdlib.h>
#include "idmap.h"

#define NUM_ELEMS 5

int main(int argc, char** argv){
	int num_elements=NUM_ELEMS;
	int id_array[NUM_ELEMS]={1,2,3,4,5};
	int content_array[NUM_ELEMS]={6,7,8,9,10};
	int i,j,res,search_id,correct;
	idmap *map=NULL;
	printf("printing id and content arrays\n id_array: ");
	for(i=0;i<num_elements;i++){
		printf("%d ",id_array[i]);
	}
	printf("\ncontent_array: ");
	for(i=0;i<num_elements;i++){
		printf("%d ",content_array[i]);
	}
	printf("\n Creating the idmap object\n");
	map=create_idmap(id_array,content_array,num_elements);
	printf("id_array and content_array can now be deallocated if they have been created with malloc\n");
	printf("searching elements in the id map\n");
	for(j=0;j<num_elements;j++){
		//we emulate the search for a pseudorandom id
		search_id=rand()%(num_elements+2);
		res=idmap_search(map,search_id,num_elements);
		correct=IDMAP_CONTENT_NOT_FOUND;
		for(i=0;i<num_elements;i++){
			if(id_array[i]==search_id){
				correct=content_array[i];
			}
		}
		printf("searched id: %d, returned value:%d correct answer:%d\n",search_id,res,correct);
		srand(search_id+res+correct);
	}
	printf("destroying idmap\n");
	destroy_idmap(map);
}
