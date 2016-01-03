#include <cstdio>
#include <cstring>
#include <chrono>
#include <iostream>
#include <exception>
#include "sensors_inf.h"


sensors_inf::sensors_inf(Queue<saved_msg*>& cmd_q, Queue<saved_msg*>& sta_q)
  : cmd_q(cmd_q), sta_q(sta_q)
{
  m_t = std::thread(&sensors_inf::command_consume, this);
  m_par = parser();
  m_db = db();

	 //m_ra = new xbeemodule();
	m_ra = new rf24module();

  
};

sensors_inf::~sensors_inf()
{
  m_ra->stop();
  m_t.join();
  delete m_ra;
  std::cout<< "<sensors_inf> sensors_inf destructed\n";
}


void sensors_inf::loop()
{
  try{
    //std::this_thread::sleep_for (std::chrono::milliseconds(100));
    //std::this_thread::sleep_for (std::chrono::seconds(1));
    
    sensor_msg rx;
    
    m_ra->recv(rx);
//    saved_msg* temp = new saved_msg;
//    
//  	strcpy(temp->topic, "test/topic/haha/hehe/1");
//  	strcpy(temp->payload, "0");
//    
//    status_produce(temp);
  }
  catch (std::exception& e){
      std::cout << e.what() << std::endl;
  }    
  std::this_thread::sleep_for (std::chrono::milliseconds(10));
}

//void sensors_inf::operator()(int n) { 
//    while(true) { 
//        std::unique_lock<std::mutex> l(_m); 
//        _c.wait(l); 
//        if(!_q.empty()) { 
//            { 
//                std::unique_lock<std::mutex> l(_mm); 
//                std::cerr << "#" << n << " " << _q.back() <<std::endl; 
//            } 
//            _q.pop(); 
//        } 
//    } 
//} 

//void sensors_inf::on_message(const sensor_msg *message)
//{
//	double temp_celsius, temp_farenheit;
//	char buf[51];
	
	//printf("on message %s\n", (char*)message->topic);
	//printf("on message %s\n", (char*)message->payload);

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
//}


void sensors_inf::command_consume(){
  
  saved_msg* s;
  sensor_msg tx;
  char addr[64];
  
  while (!signaled){
      try{
          s = cmd_q.pop();
          std::cout << "<sensors_inf> topic from cmd queue: " << s->topic << "\n";
          std::cout << "<sensors_inf> payload from cmd queue: " << s->payload << "\n";
            
          //TODO: parse the command and send it to the sensors.
          
          memset(&tx, 0, sizeof(sensor_msg));
          
          std::string topic(s->topic);
          memset(addr, 0, 64);
          
          m_par.parse(topic, addr, tx.subid);
          
          std::cout << "<sensors_inf>  addr: " << addr << "\n"; 
          tx.id = m_db.queryid(addr);
          tx.status = atoi(s->payload);
          
//          std::cout << " id: " << static_cast<unsigned>(tx.id) << "\n"; //this is the way to cout a char
//          std::cout << " subid: " << static_cast<unsigned>(tx.subid) << "\n"; //this is the way to cout a char
//          
//          //strcpy(tx.payload, s->payload);
//          //std::cout << " payload: " << tx.payload << "\n"; 
//          //std::cout << " s->payload: " << s->payload << "\n"; 
//          std::cout << " status: " << static_cast<unsigned>(tx.status) << "\n"; 
            
          m_ra->send(tx);
          saved_msg* temp = new saved_msg;
          
        	strcpy(temp->topic, "test/topic/haha/hehe/1");
        	strcpy(temp->payload, "1");
          
          status_produce(temp);
          
          delete s;
          
      }
      catch (std::exception& e){
          std::cout << e.what() << std::endl;
      }
  }
  
  saved_msg* temp;
  while (cmd_q.cleanup(temp))
  {
     std::cout << "<sensors_inf> cleaning up command queue..." << std::endl;
     delete temp;
  }
    
}


void sensors_inf::status_produce(saved_msg* s){
  printf("<sensors_inf> status_produce %s\n", (char*)s->topic);
  printf("<sensors_inf> status_produce %s\n", (char*)s->payload);

  sta_q.push(s);
}