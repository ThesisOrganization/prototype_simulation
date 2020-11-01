/** \file setup_send_functions.h
 * \brief Setup functions that send structures to other LPs.
 */
#ifndef SETUP_SEND_FUNCTIONS_H
#define SETUP_SEND_FUNCTIONS_H

#include "../../../utils/application_datatypes.h"

/** \brief Sends the info required to fille the lp_state struct.
 * \param[in] lp The lp whose info we want to send.
 * \param[in] tot_top The total topology, used to get the info to send.
 */
void send_lp_info(int lp,total_topology* tot_top);

/** \brief Sends a general_topology to an LP identified by element_id
 * \param[in] gen_top The general_topology to send.
 * \param[in] element_id The id of __both__ the LP and the element.
 */
void send_general_topology(general_topology* gen_top,int element_id);
#endif
