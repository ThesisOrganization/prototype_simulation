/** \file idmap.h
 * \brief A module which implements an integer to integer map.
*/

#ifndef IDMAP_H
#define IDMAP_H

///Value to be returned if during a search the id is not found in the given map.
#define IDMAP_CONTENT_NOT_FOUND -1

///An element of the map.
typedef struct{
	int id; ///< The id used to locate the element.
	int content; ///< The content identified by the id.
} idmap;

/** \brief Creates a new id map.
 * \param[in] id_arr The array of the ids.
 * \param[in] content_arr The array of the content to be mapped.
 * \param[in] num_elements The number of elements of both arrays.
 * id_arr and content_arr must of the same size and in the same order otherwise the mapping won't work as intended.
 */
idmap* create_idmap(int* id_arr,int* content_arr,int num_elements);

/** \brief Given an id, searches the content to be mapped.
 * \param[in] map The map where the search is done.
 * \param[in] id The of the element to be located in the mapped.
 * \param[in] num_elements The number of elements of the idmap.
 * \returns The content mapped with the given id, ::IDMAP_CONTENT_NOT_FOUND is given if the element is not found or if there is an error.
 */
int idmap_search(idmap* map,int id,int num_elements);

/** \brief Deallocates the id map.
 * \param[in] map The id map to deallocate.
 */
void destroy_idmap(idmap* map);
#endif
