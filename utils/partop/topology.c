#include <stdio.h>
#include "header.h"

//for now extremely simple function, just returning the node to send the data, will get messier.
int getNext(struct topArray * array, int index)
{
  return(array[index].receiver);
}

char * getInfo(struct topArray * array, int index)
{
  return(array[index].info);
}
