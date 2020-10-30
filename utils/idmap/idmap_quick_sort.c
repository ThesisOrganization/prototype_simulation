#include "idmap_quick_sort.h"
/// A utility function to swap two elements
void swap(idmap* a, idmap* b)
{
	idmap t = *a;
	*a = *b;
	*b = t;
}

/** This function takes last element as pivot, places
 * the* pivot element at its correct position in sorted
 * array, and places all smaller (smaller than pivot)
 * to left of pivot and all greater elements to right
 * of pivot */
int partition (idmap* arr, int low, int high)
{
	int pivot = arr[high].id; // pivot
	int i = (low - 1); // Index of smaller element
	int true_high=high-1;
	for (int j = low; j <= true_high; j++){
		// If current element is smaller than the pivot
		if (arr[j].id < pivot){
			i++; // increment index of smaller element
			if(arr[i].id != arr[j].id){
				swap(&arr[i], &arr[j]);
			}
		}
	}
	if(arr[i+1].id!=arr[high].id){
		swap(&arr[i + 1], &arr[high]);
	}
	return (i + 1);
}

/** The main function that implements QuickSort
 * arr --> Array to be sorted,
 * low --> Starting index,
 * high --> Ending index */
void quickSort(idmap* arr, int low, int high)
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
void quicksort(idmap* arr,int num){
	quickSort(arr,0,num-1);
}
