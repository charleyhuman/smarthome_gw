#ifndef RADIO_H
#define RADIO_H


#include <mutex> 
#include <condition_variable> 

#include "msg.h"



class radio
{
	public:
		radio(){}
		virtual ~radio(){}

	 virtual void stop() = 0;
	 virtual void send(sensor_msg& tx) = 0;
	 virtual signed char recv(sensor_msg& rx) = 0;
	 //virtual void send() = 0;
	 
};

#endif
