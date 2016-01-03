#include <cstdio>
#include <cstring>
#include <iostream>

#include "openhab_inf.h"


#include <mosquittopp.h>

openhab_inf::openhab_inf(const char *id, const char *host, int port, Queue<saved_msg*>& cmd_q, Queue<saved_msg*>& sta_q)
   : mosquittopp(id), cmd_q(cmd_q), sta_q(sta_q)
{
	int keepalive = 60;

	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);
	
  m_t = std::thread(&openhab_inf::status_consume, this);

};

openhab_inf::~openhab_inf()
{
  m_t.join();
  std::cout<< "<openhab_inf> openhab_inf destructed\n";
}

void openhab_inf::on_connect(int rc)
{
	printf("<openhab_inf> Connected with code %d.\n", rc);
	if(rc == 0){
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, "hello/world");
	}
}

void openhab_inf::on_message(const struct mosquitto_message *message)
{
//	double temp_celsius, temp_farenheit;
//	char buf[51];
	
	if(message!=NULL){
  	
  	saved_msg* temp = new saved_msg;
  	
  	strcpy(temp->topic, (char*)message->topic);
  	strcpy(temp->payload, (char*)message->payload);
  	
  	command_produce(temp);
  	
  }
  else{ //test
  	saved_msg* temp = new saved_msg;
  	
  	strcpy(temp->topic, "test/topic/haha/hehe/1");
  	strcpy(temp->payload, "1");
  	
  	command_produce(temp);
    
  }
	

//	if(!strcmp(message->topic, "temperature/celsius")){
//		memset(buf, 0, 51*sizeof(char));
//		/* Copy N-1 bytes to ensure always 0 terminated. */
//		memcpy(buf, message->payload, 50*sizeof(char));
//		temp_celsius = atof(buf);
//		//printf("celsius: %f\n", temp_celsius);
//		temp_farenheit = temp_celsius*9.0/5.0 + 32.0;
//		snprintf(buf, 50, "%f", temp_farenheit);
//		publish(NULL, "temperature/farenheit", strlen(buf), buf);
//	}

}

void openhab_inf::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("<openhab_inf> Subscription succeeded.\n");
}

void openhab_inf::command_produce(saved_msg* s){
  printf("<openhab_inf> command_produce %s\n", (char*)s->topic);
  printf("<openhab_inf> command_produce %s\n", (char*)s->payload);

  cmd_q.push(s);
}


void openhab_inf::status_consume(){
  
  while (!signaled){
      try{
          saved_msg* s = sta_q.pop();
          std::cout << "<openhab_inf> topic from sta queue: " << s->topic << "\n";
          std::cout << "<openhab_inf> payload from sta queue: " << s->payload << "\n";
            
          //TODO: send the status to the openhab.
          
          
          delete s;
          
      }
      catch (std::exception& e){
          std::cout << e.what() << std::endl;
      }
  }
  
  saved_msg* temp;
  while (sta_q.cleanup(temp))
  {
     std::cout << "<openhab_inf> cleaning up status queue..." << std::endl;
     delete temp;
  }
    
}
