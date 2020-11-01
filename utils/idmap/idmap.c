/** \file idmap.c
 * Implementation of the id map module.
 */
#include "idmap.h"
#include "idmap_quick_sort.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** \brief Binary search for the content with the supplied id.
 * \param[in] map The map where the search must be done.
 * \param[in] search_id The id to search for.
 * \param[in] num_elements The number of elements in the map.
 * \returns ::IDMAP_CONTENT_NOT_FOUND when the matching id is not found or the matching integer.
 * IDs are assumed to be in ascending order.
 */
static int binary_search(idmap* map,int search_id,int num_elements){
	int first,last,middle, content;
	//we initialize content to the error value
	content=IDMAP_CONTENT_NOT_FOUND;
	first = 0;
	last = num_elements - 1;
	middle = (first+last)/2;
	//we start the search
	while (first <= last && content==IDMAP_CONTENT_NOT_FOUND) {
		if (map[middle].id < search_id){
			first = middle + 1;
		} else {
			//we update content only when the matching id is found
			if (map[middle].id == search_id) {
				content=map[middle].content;
			}	else {
				last = middle - 1;
			}
		}
		middle = (first + last)/2;
	}
	return content;
}

idmap* create_idmap(int* id_arr,int* content_arr,int num_elements,int* map_length){
	idmap *map=NULL;
	int i,num_valid_content=0, num_valid_id=0;
	//some sanity checks
	if(num_elements>0 && id_arr!=NULL && content_arr!=NULL){
		// we need to get the number of valid elements and id
		for(i=0;i<num_elements;i++){
			if(content_arr[i]>=0){
				num_valid_content++;
			}
			if(id_arr[i]>=0){
				num_valid_id++;
			}
		}
		if(num_valid_content>num_valid_id){
			printf("the number of valid id does not match the number of valid elements\n");
			exit(EXIT_FAILURE);
		}
		//we give to the user the length of the map.
		*map_length=num_valid_content;
		//printf("num valid %d\n",num_valid_content);
		///The map is allocated as a contiguous array to be compatible with the setup protocol used in ROOT-Sim.
		map=malloc(sizeof(idmap)*num_valid_content);
		memset(map,0,sizeof(idmap)*num_valid_content);
		//we map the content to the ids, excluding the invalid values.
		int counter = 0;
		for(i=0;i<num_elements;i++){
			if(id_arr[i]>=0 && content_arr[i]>=0){
				map[counter].id=id_arr[i];
				map[counter].content=content_arr[i];
				counter+=1;
			}
		}
		///The elements of the idmap array are ordered (in acending order) by their id using the quicksort algorithm
		idmap_quicksort(map,num_valid_content);
	}
	return map;
}

int idmap_search(idmap* map,int id,int num_elements){
	int content=IDMAP_CONTENT_NOT_FOUND;
	if(map!=NULL){
		///We use a binary search to locate the element of the array that contains the specified id.
		content=binary_search(map,id,num_elements);
	}
	return content;
}

void destroy_idmap(idmap* map){
	//since the array is contiguous a single free is sufficient to clear the memory.
	free(map);
}
