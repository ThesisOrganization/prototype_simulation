/** \file message.c
 * Implementation of the message functions.
 */
#include "message.h"

void* create_setup_message(int device_id,setup_data_types container,setup_data_types data_type, void* data,size_t data_size,size_t *message_size){
	void* message=NULL,*m_data=NULL;
	message_setup *m_info=NULL;
	*message_size=0;
	//some sanity checks
	if(device_id >=0 && data!=NULL){
		///We need to use a contiguous memory block for both the message_setup struct and the data
		if(data_size>=0){
			message=malloc(sizeof(data_size)+sizeof(message_setup));
		}
		//we can now set the payload_size
		*message_size=sizeof(message_setup)+data_size;
		/// We write the message_info struct at the beginning of the memory zone, then we copy the data.
		m_info=message;
		m_info->header.element_id=device_id;
		m_info->data_type=data_type;
		m_info->container_struct=container;
		m_info->data_size=data_size;
		//after having written the message_setup struct we can copy the data
		if(data_size>=0){
			m_data=message+sizeof(message_size);
		}
		///We make the data directly accessible from the message_setup struct, but this is the same memory zone only with
		m_info->data=m_data;
		if(data_size>=0){
			memcpy(m_data,data,data_size);
		}
	}
	return message;
}

message_setup* get_message_info(void* message){
/// The message setup struct is at the beginning of the payload, so a cast is enough
	return (message_setup*) message;
}

void* get_message_data_void(void* message){
	///The data portion begins right after the payload, so we need to use the pointer's algebra to reach it.
	return message+sizeof(message_setup);
}

void destroy_message(void* message){
	free(message);
}

