#include "priority_scheduler.h"
/// A utility function to swap two elements
void swap(queue_conf** a, queue_conf** b)
{
	queue_conf* t = *a;
	*a = *b;
	*b = t;
}

/** This function takes last element as pivot, places
 * the* pivot element at its correct position in sorted
 * array, and places all smaller (smaller than pivot)
 * to left of pivot and all greater elements to right
 * of pivot */
int partition (queue_conf** arr, int low, int high)
{
	int pivot = arr[high]->type; // pivot
	int i = (low - 1); // Index of smaller element

	for (int j = low; j <= high - 1; j++){
		// If current element is smaller than the pivot
		if (arr[j]->type < pivot){
			i++; // increment index of smaller element
			if(arr[i]->type != arr[j]->type){
				swap(&arr[i], &arr[j]);
			}
		}
	}
	if(arr[i+1]->type!=arr[high]->type){
		swap(&arr[i + 1], &arr[high]);
	}
	return (i + 1);
}

/** The main function that implements QuickSort
 * arr --> Array to be sorted,
 * low --> Starting index,
 * high --> Ending index */
void quickSort(queue_conf** arr, int low, int high)
{
	int pi;
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		 *     at right place */
		pi = partition(arr, low, high);

		// Separately sort elements before
		// partition and after partition
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

/// Wrapper of the recursive quickSort function.
void quicksort(queue_conf** arr,int num){
	quickSort(arr,0,num-1);
}
