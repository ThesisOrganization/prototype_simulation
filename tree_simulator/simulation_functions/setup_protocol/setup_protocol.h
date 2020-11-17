/** \file setup_protocol.h
 * \brief LP setup functions.
 * The setup protocolo relies heavily on the fact that events are received by all LP in ordered by timestamps.
 * Furthermore both the send and the receive functions rely on the fact that inner pointers in any struct are sent in the order the are declared.
 */
#ifndef SETUP_PROTOCOL_H
#define SETUP_PROTOCOL_H

/// This macro is used to switch setup with minimal code changes. If you use the "setup" symbol by default it will use the setup_master function. If you define the SETUP_INDEPENDENT symbol it will use the setup_independent function.
#ifdef SETUP_INDEPENDENT
	#define setup(me,state,n_prc_tot) setup_independent(me,state,n_prc_tot)
#else
#define setup(me,state,n_prc_tot) \
	do { \
		if(me == 0){ \
			setup_master(n_prc_tot);\
		}\
	}while(0);
#endif

#include "../common_functions.h"
#include "setup_recv_functions.h"

/** \brief Reads the topology file and sends topology information to the other LPs.
 * \param[in] n_prc_tot The number of given LPs.
 */
void setup_master(int n_prc_tot);

/** \brief Alternative setup method which dose not rely on messages between LPs.
 * \param[in] lp The LP id.
 * \param[in] state The LP state.
 * \param[in] n_prc_tot The number of given LPs.
 */
void setup_independent(int lp,lp_state* state,int n_prc_tot);

#endif /* SETUP_PROTOCOL_H */
