#ifndef MSG_H
#define MSG_H

typedef struct
{
  char topic[64];
  char payload[16];
  
} saved_msg;


//typedef struct
//{
//  char addr[64];
//  char id;
//  char payload[16];//command or status
//  
//} sensor_msg;

typedef struct
{
  unsigned char id;
  unsigned char subid;
  unsigned char status; //command or status
  
} sensor_msg;


#endif