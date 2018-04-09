#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/robot.h>

#include "com.h"
#include "consts.h"

WbDeviceTag emitter;
WbDeviceTag receiver;

void com_init()
{
    emitter = wb_robot_get_device("emitter");
    wb_emitter_set_channel(emitter, COM_CHANNEL);
    
    receiver = wb_robot_get_device("receiver");
    wb_receiver_enable(receiver, TIME_STEP);
}

void com_send(packet p)
{
    wb_emitter_send(emitter, p.data, (int) p.size);
}

packet com_receive()
{
    packet p;
    
    if(wb_receiver_get_queue_length(receiver) > 0)
    {
        p.data = wb_receiver_get_data(receiver);
        p.size = (size_t) wb_receiver_get_data_size(receiver);
    }
    else
    {
        p.data = NULL;
    }
    
    wb_receiver_next_packet(receiver);
    
    return p;
}
