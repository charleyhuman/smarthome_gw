#ifndef SENSORS_INF_H
#define SENSORS_INF_H

#include <thread> 
#include <queue> 
#include <mutex> 
#include <condition_variable> 
#include <atomic>
#include "queue.h"
#include "parser.h"
#include "msg.h"
#include "radio.h"
#include "nrf24l01.h"
#include "db.h"


extern std::atomic<bool> signaled;





class sensors_inf //consumer for command queue, and producer for status queue
{
	public:
		sensors_inf(Queue<saved_msg*>& cmd_q, Queue<saved_msg*>& sta_q);
		~sensors_inf();
//		operator()(int n);

//		void on_message(const sensor_msg *message);
		void loop();
	private:	
    Queue<saved_msg*>& cmd_q;//consumer
    std::thread m_t;
    void command_consume();
    void status_produce(saved_msg*);
    
    Queue<saved_msg*>& sta_q;//producer
    
    parser m_par;//
    radio* m_ra;
    db m_db;
    //  Only needed to synchronize I/O 
    //static std::mutex       _mm; 
    // Reader may write into our queue 
    //friend class reader; 
};

#endif
