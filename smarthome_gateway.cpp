#include <iostream>
#include <chrono>
#include <cstring>
#include <signal.h>
#include <atomic>

#include "openhab_inf.h"
#include "sensors_inf.h"
#include "config.h"

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

#include <queue>
#include <random>

//std::mutex              g_lockprint;
//std::mutex              g_lockqueue;
//std::condition_variable g_queuecheck;
//std::queue<int>         g_codes;
//bool                    g_done;
//bool                    g_notified;
//
//void command_producer(int id)
//{
//    // print a starting message
//    {
//        //std::unique_lock<std::mutex> locker(g_lockprint);
//        //std::cout << "[producer " << id << "]\trunning..." << std::endl;
//    }
//
//    // simulate work
//    //std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));
//
//    // simulate error
//    int errorcode = id;
//    {
//        //std::unique_lock<std::mutex> locker(g_lockprint);
//        std::cout << "[producer " << id << "]\tan error occurred: " << errorcode << std::endl;
//    }
//
//    // notify error to be logged
//    {
//        cout << "[producer]\ttry to get the lock\n";
//        std::unique_lock<std::mutex> locker(g_lockqueue);
//        cout << "[producer]\tget the lock\n";
//        g_codes.push(errorcode);
//        g_queuecheck.notify_one();
//    }
//}
//
//void command_consumer()
//{
//    // print a starting message
//    {
//        //std::unique_lock<std::mutex> locker(g_lockprint);
//        //std::cout << "[consumer]\trunning..." << std::endl;
//    }
//
//    // loop until end is signaled
//    while(!signaled)
//    {
//        std::unique_lock<std::mutex> locker(g_lockqueue);
//        cout<< "[consumer]\t waiting...\n";
//        g_queuecheck.wait(locker, [&](){return !g_codes.empty() || signaled;});
//        cout<< "[consumer]\t wakes up...\n";
//
//        // if there are error codes in the queue process them
//        if(!g_codes.empty())
//        {
//            //std::unique_lock<std::mutex> locker(g_lockprint);
//            std::cout << "[consumer]\tprocessing error:  " << g_codes.front() << std::endl;
//            g_codes.pop();
//            locker.unlock();
//            std::this_thread::sleep_for (std::chrono::milliseconds(500));
//        }
//    }
//    while(!g_codes.empty())
//    {
//        //std::unique_lock<std::mutex> locker(g_lockprint);
//        std::cout << "[consumer]\tprocessing error:  " << g_codes.front() << std::endl;
//        g_codes.pop();
//        //locker.unlock();
//        std::this_thread::sleep_for (std::chrono::milliseconds(500));
//    }
//}



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
  Queue<saved_msg*> command_queue;
  Queue<saved_msg*> status_queue;

  install_sig_hooks();
  
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

