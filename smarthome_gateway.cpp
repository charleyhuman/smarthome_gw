#include <iostream>
#include <chrono>
#include <cstring>
#include <signal.h>
#include <atomic>
#include <queue>
#include <random>

#include "openhab_inf.h"
#include "sensors_inf.h"
#include "config.h"

#include "radio.h"
#include "nrf24l01.h"

using namespace std;

//static volatile int signaled = 0;
atomic<bool> signaled;

void sighandler(int sig, siginfo_t *siginfo, void *context) {
  //cout << "signaled is set\n";
  signaled = true;
}

void install_sig_hooks() {
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_sigaction = sighandler;
  action.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &action, NULL);
}


void openhab_main (Queue<saved_msg*>& cmd_q, Queue<saved_msg*>& sta_q) //fetch data from openhub
{
	int rc = 0;

	mosqpp::lib_init();
	class openhab_inf openhab(NULL, "192.168.1.3", 1883, cmd_q, sta_q);
  //static int i = 0;
	
	while(!signaled){
	  //command_producer(++i);
#if OPENHAB_TESTING
	  openhab.on_message(NULL);
	  std::this_thread::sleep_for (std::chrono::milliseconds(500));
#else
		rc = openhab.loop();
		if(rc){
			openhab.reconnect();
		}
#endif

	}
	
	mosqpp::lib_cleanup();

}


void sensors_main (Queue<saved_msg*>& cmd_q, Queue<saved_msg*>& sta_q) //fetch data from sensors
{
  class sensors_inf sensors(cmd_q, sta_q);// adding () is not correct!
  //std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
  
  while(!signaled){
    //cout << "test2\n";
    //command_producer(generator() % 5);
    
    sensors.loop();
    //std::this_thread::yield();
  }
  
}

void guardian_main ()
{
	while(!signaled){
	  std::this_thread::sleep_for (std::chrono::milliseconds(100));
	}
	
}



int main(int argc, char *argv[])
{
                   
  signaled = false;
  install_sig_hooks();
  
  Queue<saved_msg*> command_queue;
  Queue<saved_msg*> status_queue;
  
  std::thread openhab_thread (openhab_main, std::ref(command_queue), std::ref(status_queue));
  std::thread sensors_thread (sensors_main, std::ref(command_queue), std::ref(status_queue));
  std::thread guardian_thread (guardian_main); //used for thread synchronization
  //std::thread command_consumer_thread (command_consumer);
    
  guardian_thread.join(); //wait until this dummy thread end.
  
  //g_queuecheck.notify_one();

  command_queue.stop();
  status_queue.stop();
  
  openhab_thread.join(); 
  sensors_thread.join(); 
  //command_consumer_thread.join();

  
  cout << "<main> main thread quit\n";
   
  
	return 0;
}

