/** \file setup_protocol.h
 * \brief LP setup functions.
 * The setup protocolo relies heavily on the fact that events are received by all LP in ordered by timestamps.
 * Furthermore both the send and the receive functions rely on the fact that inner pointers in any struct are sent in the order the are declared.
 */
#ifndef SETUP_PROTOCOL_H
#define SETUP_PROTOCOL_H

#include "../common_functions.h"
#include "setup_recv_functions.h"

/** \brief Reads the topology file and sends topology information to the other LPs.
 * \param[in] n_prc_tot The number of given LPs.
 */
void setup_master(int n_prc_tot);

#endif /* SETUP_PROTOCOL_H */
