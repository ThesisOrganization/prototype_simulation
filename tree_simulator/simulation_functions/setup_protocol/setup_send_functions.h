/** \file setup_send_functions.h
 * \brief Setup functions that send structures to other LPs.
 */
#ifndef SETUP_SEND_FUNCTIONS_H
#define SETUP_SEND_FUNCTIONS_H
#include "../common_functions.h"

/** \brief Sends an Element_topology struct to the destination LP.
 * \param[in] tot_top Total topology struct.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * \param[in] total_elements The total number of elements in the topology.
 */
void send_element_topology(total_topology* tot_top,int element_id, int total_elements);

/** \brief Sends a general_topology to an LP identified by element_id
 * \param[in] gen_top The general_topology to send.
 * \param[in] element_id The id of __both__ the LP and the element.
 */
void send_general_topology(general_topology* gen_top,int element_id);
#endif
