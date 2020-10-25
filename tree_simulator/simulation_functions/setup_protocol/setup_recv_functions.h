#ifndef SETUP_RECV_FUNCTIONS_H
#define SETUP_RECV_FUNCTIONS_H
/** \file setup_recv_functions.h
 * \brief Functions that will be called by RECEIVE_SETUP_INFO and RECEIVE_SETUP_DATA.
 */

#include "../common_functions.h"

/** \brief receives setup info and saves it into the LP state.
 * \param[in] info The setup info to be saved.
 * \param[in] state The LP state.
 */
void recv_setup_info(setup_info* info, lp_state* state);

/** \brief Uses the setup_info in the lp state to identify the data received and places accordingly in the state.
 * \param[in] state The LP state-
 * \param[in] data The data received, which needs to be placed into the state.
 */
void recv_setup_data(lp_state* state, void* data);

#endif
