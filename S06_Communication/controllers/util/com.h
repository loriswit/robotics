#ifndef ROBOTICS_COM_H
#define ROBOTICS_COM_H

#include <stdlib.h>

#define COM_CHANNEL 1

typedef struct
{
    void * data;
    size_t size;
} packet;

void com_init();

bool com_send(packet p);

packet com_receive();

#endif //ROBOTICS_COM_H
