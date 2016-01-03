#ifndef RF24_H
#define RF24_H


#include <RF24.h>
#include "msg.h"
#include "config.h"

class rf24module : public radio  //consumer for command queue, and producer for status queue
{
	public:
		rf24module() : m_radio(115, 117), m_isProgramStopped(false), m_isbusy(false)
		{
		  
#if RF24_TESTING
      
      std::cout << "<testing> <rf24> initializing... done!\n";

#else		  
      std::cout << "<rf24> initializing...";
        
      m_radio.begin();
    
      // optionally, increase the delay between retries & # of retries
      m_radio.setRetries(15,15);
    
      // optionally, reduce the payload size.  seems to
      // improve reliability
    //  radio.setPayloadSize(8);
      m_radio.setChannel(0x70); //2400 + 112 MHz = 2.512 GHz
      m_radio.setPALevel(RF24_PA_MAX);
      //radio.setDataRate(RF24_250KBPS);
    
      //
      // Open pipes to other nodes for communication
      //
    
      // This simple sketch opens two pipes for these two nodes to communicate
      // back and forth.
      // Open 'our' pipe for writing
      // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
    
//      if ( role == role_ping_out )
//      {
      m_radio.openWritingPipe(pipes[0]);
      m_radio.openReadingPipe(1,pipes[1]);
//      }
//      else
//      {
//        m_radio.openWritingPipe(pipes[1]);
//        m_radio.openReadingPipe(1,pipes[0]);
//      }
    
      m_radio.setAutoAck(true);
      m_radio.enableAckPayload();
      // Allow optional ack payloads
      
    
      m_radio.startListening();
      //m_radio.stopListening();
    
      //
      // Dump the configuration of the rf unit for debugging
      //
      std::cout << "done!\n";

      m_radio.printDetails();	
      
#endif		  
		  
		}
		~rf24module()
		{
		  //m_isProgramStopped = true;
      //m_cond.notify_all();
      std::cout<< "<rf24> rf24 destructed\n";
		}
		
		void stop()
		{
		  m_isProgramStopped = true;
      m_cond.notify_all();
      std::cout<< "<rf24> rf24 stop()\n";
		}    
		
    void send(sensor_msg& tx)
    {
      std::cout << "<rf24>  id: " << static_cast<unsigned>(tx.id) << "\n"; //this is the way to cout a char
      std::cout << "<rf24>  subid: " << static_cast<unsigned>(tx.subid) << "\n"; //this is the way to cout a char
      std::cout << "<rf24>  status: " << static_cast<unsigned>(tx.status) << "\n"; 
        
      //std::this_thread::sleep_for (std::chrono::seconds(1));//simulate sending...
      std::cout << "<rf24> try to acquire the lock in send()... \n";
      std::unique_lock<std::mutex> lock(m_mtx);
      std::cout << "<rf24> waiting in send()... \n";
      m_cond.wait(lock, [&](){ return !m_isbusy || m_isProgramStopped; });
      std::cout << "<rf24> got the lock... \n";
      
      if (m_isProgramStopped){
          throw std::runtime_error("Program stopped in rf24 send!");
      }
      
      m_isbusy = true;

#if RF24_TESTING

      std::this_thread::sleep_for (std::chrono::milliseconds(100));//simulate sending...    

#else

      // First, stop listening so we can talk.
      m_radio.stopListening();
      
  
      // Take the time, and send it.  This will block until complete
      unsigned long time = __millis();
      printf("<rf24> Now sending %lu...",time);

      
      bool ok = m_radio.write( &tx, sizeof(sensor_msg) );
      
      if (ok)
        printf("ok...");
      else
        printf("failed.\n\r");
  
      // Now, continue listening
      m_radio.startListening();
  
      // Wait here until we get a response, or timeout (250ms)
      unsigned long started_waiting_at = __millis();
      bool timeout = false;
      while ( ! m_radio.available() && ! timeout ) {
  	// by bcatalin ? Thu Feb 14, 2013 11:26 am 
  	__msleep(5); //add a small delay to let radio.available to check payload
        if (__millis() - started_waiting_at > 200 )
          timeout = true;
      }
  
      // Describe the results
      if ( timeout )
      {
        printf("Failed, response timed out.\n\r");
      }
      else
      {
        memset(&tx, 0, sizeof(sensor_msg));
        m_radio.read( &tx, sizeof(sensor_msg) );
        printf("<rf24> Got id %u...",tx.id);
        printf("<rf24> Got subid %u...",tx.subid);
        printf("<rf24> Got status %u...\n\r",tx.status);
          
      }
      
#endif
      std::cout << "<rf24> send done... \n";

      m_isbusy = false;
      m_cond.notify_one();
      std::cout << "<rf24> send() quit... \n";
        
    }

    void recv(sensor_msg& rx)
    {
      //std::cout << "<rf24> try to acquire the lock in recv()... \n";
      std::unique_lock<std::mutex> lock(m_mtx);
      //std::cout << "<rf24> waiting in recv()... \n";
      m_cond.wait(lock, [&](){ return !m_isbusy || m_isProgramStopped; });
      //std::cout << "<rf24> got the lock... \n";
      
      if (m_isProgramStopped){
          throw std::runtime_error("Program stopped in rf24 recv!");
      }
      
      m_isbusy = true;
      
#if RF24_TESTING

      std::this_thread::sleep_for (std::chrono::milliseconds(100));//simulate recving...    

#else

      m_radio.startListening();

      // if there is data ready
      if ( m_radio.available() )
      {
        // Dump the payloads until we've gotten everything
        unsigned char err = 0;
        bool done = false;
        while (!done)
        {
          // Fetch the payload, and see if this was the last one.
          done = m_radio.read( &rx, sizeof(sensor_msg) );
  
          // Spew it
          //printf("<rf24> Got payload %lu...",got_time);
  
  	// Delay just a little bit to let the other unit
  	// make the transition to receiver
  	delay(20);
        }
  
        // First, stop listening so we can talk
        m_radio.stopListening();
  
        // Send the final one back.
        printf("<rf24> Sent response.\n\r");
        m_radio.write( &err, sizeof(unsigned char) );
  
        // Now, resume listening so we catch the next packets.
        m_radio.startListening();
      }
#endif      
      //std::cout << "<rf24> recv done... \n";

      m_isbusy = false;
      m_cond.notify_one();
      //std::cout << "<rf24> recv() quit... \n";
      
    }
    

	private:
	  RF24 m_radio;
	  const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
    std::mutex m_mtx;
    std::condition_variable m_cond;
    bool m_isProgramStopped;
    bool m_isbusy;


};

#endif
