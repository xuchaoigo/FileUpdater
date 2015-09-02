#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <string>
#include <iostream>
#include "typedefs.h"

#define MAX_BUFFER_LEN 1024
#define HTTP_OK 0
#define HTTP_ERROR -1
#define NO_SOCKET -1
#define HTTP_TRUE true
#define HTTP_FALSE false

typedef struct httpsession
{
	INT sock;              //socket for connection
	ULONGLONG cseq_tx;  //transmission seq num
} httpsession_t;

typedef struct serverinfo
{
	std::string ip;
	UINT port;
} serverinfo_t;

class MyHttp
{
public:
   	MyHttp(std::string serverIp, UINT serverPort);
  	~MyHttp();         
	BOOL establishConnection();
	VOID destroyConnection();
	BOOL reConnection();
	INT httpSnd();
	INT httpRcv();

	PCSTR test_send_recv();


private:
	INT send_command();
	INT get_response();
	INT get_response_timeout(int sec, int usec);
	
	httpsession_t  thisSession;
	serverinfo_t serverInfo;

    CHAR tx_buffer[MAX_BUFFER_LEN];      //buffer to transmit data
    CHAR rx_buffer[MAX_BUFFER_LEN];      //buffer to receive data

};
