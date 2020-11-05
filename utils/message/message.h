/** \file message.h
 * The following file contains the functions used to create a message payload and for retrieving content from said payload.
 */
#ifndef SETUP_PAYLOAD_H
#define SETUP_PAYLOAD_H

#include"../application_datatypes.h"

/** \brief Create a setup protocol message payload.
 * \param[in] device_id The id of the topology device we are setting up.
 * \param[in] container The struct that contains the data we are preparing.
 * \param[in] data_size The size of the data that we are about to send.
 * \param[in] data The data we want to send.
 * \param[out] payload_size The size of the payload.
 * \returns The payload to be sent, which combines all the above data or NULL in case of invalid parameters.
 */
void* create_setup_message(int device_id,setup_data_types container,setup_data_types data_type, void* data,size_t data_size,size_t *message_size);


/** \brief Retrieves the message_setup from the given payload.
 * \param[in] payload The payload from where we want to extract the message_setup struct.
 * \returns The message_setup struct.
 */
message_setup* get_setup_message_info(void* message);

/** \brief Deallocates the given payload
 * \param[in] The payload to deallocate.
 */
void destroy_message(void* message);
#endif
