#include "MyHttp.h"

MyHttp::MyHttp(std::string serverIp, UINT serverPort)
{
	serverInfo.ip = serverIp;
	serverInfo.port = serverPort;
}
MyHttp::~MyHttp()
{

}

BOOL MyHttp::establishConnection()
{
	BOOL connectionStatus = HTTP_TRUE;
	struct addrinfo hints, *res, *resave;
	UINT error;
	INT iret;
	UINT n;
	struct timeval timeout;
	socklen_t len;
	CHAR empty[1];
	
	empty[0] = '\0';
	strcpy(rx_buffer, empty);
	strcpy(tx_buffer, empty);
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	thisSession.sock = NO_SOCKET;
	char port[8];
	memset(port,0,8);
	sprintf(port,"%d",serverInfo.port);
	
	std::cout<<"establishConnection:"<<serverInfo.ip<<":"<< serverInfo.port<<std::endl;
	error = getaddrinfo(serverInfo.ip.c_str(), port, &hints, &res);
	if(error) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		connectionStatus = HTTP_FALSE;
		return connectionStatus;
	}
	resave = res;
	do
	{
		thisSession.sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(thisSession.sock < 2)
			continue;
		UINT on =1;
		iret = setsockopt(thisSession.sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if(iret==HTTP_ERROR)
		{
			shutdown(thisSession.sock,SHUT_RDWR);
			fprintf(stderr,"setsockopt error\n");
			return HTTP_FALSE;
		}
		ULONGLONG ul = 1;
		iret = ioctl(thisSession.sock, FIONBIO, (ULONGLONG*)&ul);
		if(iret==HTTP_ERROR)
		{
			shutdown(thisSession.sock,SHUT_RDWR);
			fprintf(stderr, "ioctl error\n");
			return HTTP_FALSE;
		}
		fd_set rset, wset;
		FD_ZERO(&rset);
		FD_SET(thisSession.sock, &rset);
		wset=rset;
		timeout.tv_sec = 2; 
		timeout.tv_usec =0;
		if((n=connect(thisSession.sock, res->ai_addr, res->ai_addrlen))<0)
		{
			if(errno!=EINPROGRESS)
			{
				shutdown(thisSession.sock,SHUT_RDWR);
				perror("error");
				return HTTP_FALSE;
			}
		}
		if(n==0)
			break;
		if((iret = select(thisSession.sock+1, &rset, &wset, NULL, &timeout))==0)
		{
			shutdown(thisSession.sock,SHUT_RDWR);
			thisSession.sock=NO_SOCKET;
			DBG("timeout\n");
			continue;
		}
		if(FD_ISSET(thisSession.sock,&rset)||FD_ISSET(thisSession.sock,&wset))
		{
			len=sizeof(error);
			if(getsockopt(thisSession.sock, SOL_SOCKET, SO_ERROR , &error, &len)<0)
			{
				shutdown(thisSession.sock,SHUT_RDWR);
				thisSession.sock=NO_SOCKET;
				continue;
			}
			break;
		}
	}while((res = res->ai_next) != NULL);
	if (res == NULL)
		connectionStatus=HTTP_FALSE;
	freeaddrinfo(resave);
	if(error)
	{
		shutdown(thisSession.sock,SHUT_RDWR);
		thisSession.sock = NO_SOCKET;
		connectionStatus = HTTP_FALSE;
	}
	if(connectionStatus)
	{
		ULONGLONG ul1= 0;
		iret = ioctl(thisSession.sock, FIONBIO, (ULONGLONG*)&ul1);
		if(iret == HTTP_ERROR)
		{
			fprintf(stderr,"ioctl error\n");
			shutdown(thisSession.sock,SHUT_RDWR);
			thisSession.sock = NO_SOCKET;
			connectionStatus = HTTP_FALSE;
		}
	}
	if (thisSession.sock < 0) 
	{
		fprintf(stderr, "can't connect to the server\n");
		connectionStatus = HTTP_FALSE;
	}
	else
	{
		DBG("[%s()] thisSession.sock %d\n", __func__, thisSession.sock);
		timeout.tv_sec = 3;
		timeout.tv_usec = 5000;
		setsockopt(thisSession.sock,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
		timeout.tv_sec = 3;
		timeout.tv_usec = 5000;
		setsockopt(thisSession.sock,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
		DBG("connect to the server\n");
	}
	return connectionStatus;
}

BOOL MyHttp::reConnection()
{
	destroyConnection();
	return establishConnection();
}

VOID MyHttp::destroyConnection()
{
	if (thisSession.sock >= 0)
	{
		DBG("close socket %d\n", thisSession.sock);
		shutdown(thisSession.sock,SHUT_RDWR);
	}
	thisSession.sock = NO_SOCKET;
}

INT MyHttp::send_command()
{
	UINT len = 0;
	UINT bytes_sent = 0;

	struct iovec data_vec[1];
	len = strlen(tx_buffer);


	data_vec[0].iov_base = tx_buffer;
	data_vec[0].iov_len = len;
	bytes_sent = writev(thisSession.sock, &(data_vec[0]), 1);
	DBG("bytes_sent=%d\n", bytes_sent);
	if(bytes_sent < len)
	{
		perror("writev() ");
		return HTTP_ERROR;
	}
	return HTTP_OK;
}

INT MyHttp::get_response()
{

	UINT bytes_recv=0;
	struct iovec data_vec[1];

	data_vec[0].iov_base = rx_buffer;
	data_vec[0].iov_len = MAX_BUFFER_LEN-1;
	bytes_recv = readv(thisSession.sock, &(data_vec[0]), 1);
	DBG("bytes_recv=%d\n", bytes_recv);
	if(bytes_recv < 0)
	{
		perror("readv():");
		return HTTP_ERROR;
	}
	rx_buffer[bytes_recv]='\0';
	return bytes_recv;
}

INT MyHttp::httpSnd()
{

	return HTTP_OK;
}
INT httpRcv()
{

	return HTTP_OK;
}

PCSTR MyHttp::test_send_recv()
{
	std::string current_version = "ver1001";
	sprintf(tx_buffer, "GET http://%s:%u\nUPDATE %s",serverInfo.ip.c_str(), serverInfo.port,\
			current_version.c_str());
	DBG("tx_buffer:\n%s\n", tx_buffer);
	if(send_command())
		return NULL;
	if(get_response() <= 0)
		return NULL;
	DBG("rx_buffer:\n%s\n", rx_buffer);
	return rx_buffer;
}

