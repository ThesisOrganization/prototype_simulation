/** \file idmap_quick_sort.h
 * \brief Quick sort algorithm to order the inpu and output queues according to their type.
 */
#ifndef QUEUE_QUICK_SORT_H
#define QUEUE_QUICK_SORT_H

#include "idmap.h"

/** \brief Quick Sort algorithm to sort in place an array of queues.
 * \todo see if its also invariant
 * \param[in,out] arr The queue array to be sorted.
 * \param[in] num The number of elements in the array.
 */
void idmap_quicksort(idmap* arr,int num);
#endif
