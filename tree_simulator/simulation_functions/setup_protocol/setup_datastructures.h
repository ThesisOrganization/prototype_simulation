/** \file setup_dataypes.h
 * \brief Data structures used by seup send and setup receive functions
 */
#ifndef SETUP_DATASTRUCTURES_H
#define SETUP_DATASTRUCTURES_H

///struct used to aggregate the devices below an element
typedef struct {
	int* actuatorsTypesBelow;
	int* sensorsTypesBelow;
	int* ListSensorsByType;
	int* ListActuatorsByType;
} below_devs;

#endif
