#ifndef SETUP_RECV_FUNCTIONS_H
#define SETUP_RECV_FUNCTIONS_H
/** \file setup_recv_functions.h
 * \brief Functions that will be called by RECEIVE_SETUP_INFO and RECEIVE_SETUP_DATA.
 */

#include "../common_functions.h"

/** \brief Handles the reception of a setup message.
 * \param[in] state The LP state.
 * \param[in] data The message received, which contains data that needs to be placed into the state.
 */
void recv_setup_message(lp_state* state, void* data);

#endif
