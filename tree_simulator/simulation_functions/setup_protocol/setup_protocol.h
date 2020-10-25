/** \file setup_protocol.h
 * \brief LP setup functions.
 */
#ifndef SETUP_PROTOCOL_H
#define SETUP_PROTOCOL_H

#include "../common_functions.h"

/** \brief Reads the topology file and sends topology information to the other LPs.
 * \param[in] n_prc_tot The number of given LPs.
 */
void setup_master(int n_prc_tot);

#endif /* SETUP_PROTOCOL_H */
