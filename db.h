#ifndef DB_H
#define DB_H

#include "msg.h"

class db
{
	public:
	  
	  
	  
    unsigned char queryid(char* addr) //by addr
    {
      unsigned char id;
      id = 13; //TODO, query sqlite by addr
      return id;
    }
    
    void queryaddr(unsigned char id, char* addr) //by id
    {
      sprintf(addr, "test/test/test");//TODO, query sqlite by id

    }	 
    
};

#endif