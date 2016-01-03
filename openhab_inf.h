#ifndef OPENHAB_INF_H
#define OPENHAB_INF_H

#include <thread> 
#include <queue> 
#include <mutex> 
#include <condition_variable> 
#include <atomic>
#include <mosquittopp.h>

#include "queue.h"
#include "msg.h"

extern std::atomic<bool> signaled;
  
  
//typedef struct
//{
//  char topic[128];
//  char payload[256];
//  
//} saved_msg;

class openhab_inf : public mosqpp::mosquittopp //producer for command queue, and consumer for status queue
{
	public:
		openhab_inf(const char *id, const char *host, int port, Queue<saved_msg*>& cmd_q, Queue<saved_msg*>& sta_q);
		~openhab_inf();

		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
		
  private:
    Queue<saved_msg*>& cmd_q;//producer
    void command_produce(saved_msg*);
    void status_consume();
    
    Queue<saved_msg*>& sta_q;
    std::thread m_t;//consumer

    //unsigned int m_counter;
};

#endif
