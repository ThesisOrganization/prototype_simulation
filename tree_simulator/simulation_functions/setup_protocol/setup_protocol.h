#ifndef SETUP_PROTOCOL_H
#define SETUP_PROTOCOL_H

#include "../common_functions.h"

/** \brief Initializes the master LP and sends topology information to the other LP.
 * \param[in] me The id of the master LP.
 * \param[in] n_prc_tot The number of LPs.
 * \param[in] master_state The state of the master LP.
 */
void setup_master(int me,int n_prc_tot,lp_state* master_state);

#endif /* SETUP_PROTOCOL_H */
