#ifndef DB_H
#define DB_H

#include "msg.h"

class db
{
	public:
	  
	  
	  
    unsigned char queryid(char* addr) //by addr
    {
      unsigned char id;
      id = 1; //TODO, query sqlite by addr
      return id;
    }
    
    void queryaddr(unsigned char id, unsigned char subid, char* addr) //by id
    {
      sprintf(addr, "/home/bedr/p1/%u/state", subid);//TODO, query sqlite by id

    }	 
    
};

#endif