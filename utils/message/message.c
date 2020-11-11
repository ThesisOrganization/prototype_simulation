/** \file message.c
 * Implementation of the message functions.
 */
#include "message.h"
#include<string.h>

void* create_setup_message(int device_id,setup_data_types container,setup_data_types data_type, void* data,size_t data_size,size_t *message_size){
	void* message=NULL;
	message_setup *m_info=NULL;
	*message_size=0;
	int size;
	//some sanity checks
	if(data!=NULL){
		///We need to use a contiguous memory block for both the message_setup struct and the data
		size=sizeof(message_setup)+data_size;
		*message_size=size;
		message=malloc(size);

		/// We write the message_info struct at the beginning of the memory zone, then we copy the data.
		m_info=message;
		m_info->header.element_id=device_id;
		m_info->data_type=data_type;
		m_info->container_struct=container;
		m_info->data_size=data_size;
		/// memory in ROOT-Sim is LP-localized, so we can't assign data before sending the message, since it would have the source LP memory address.
		m_info->data=NULL;
		//after having written the message_setup struct we can copy the data
		///We make the data directly accessible from the message_setup struct, but this is the same memory zone only with

		if(data_size>0){
			memcpy(message+sizeof(message_setup),data,data_size);
		}
	}
	return message;
}

message_setup* get_setup_message_info(void* message){
/// The message setup struct is at the beginning of the payload, so a cast is enough
	message_setup* m_info=message;
	///Since this function is called on the receiver LP we can nor assign the data param of the setup message struct, since it will use thi LP memory.
	if(m_info->data_size>0){
		m_info->data=message+sizeof(message_setup);
	}
	return m_info;
}

void destroy_message(void* message){
	free(message);
}

