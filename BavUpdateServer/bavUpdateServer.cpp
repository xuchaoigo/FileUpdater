#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "filediff/filediff.h"
using namespace std;

#define MAXSIZE 1024
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 5000
#define INFTIM 1000

void setnonblocking(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

string parseOperation(const char * line)
{
	string str = line;
	size_t pos = str.find("UPDATE");
	if(pos == string::npos)
		return "";
	else
	{
		return str.substr(pos+strlen("UPDATE")+1);
	}
}

int main(int argc, char* argv[])
{
    int i, listenfd, connfd, sockfd,epfd,nfds, portnumber;
    ssize_t readNum,writeNum;
    char line[MAXSIZE];
    socklen_t clilen;
    if ( 2 == argc )
    {
        if( (portnumber = atoi(argv[1])) < 0 )
        {
            fprintf(stderr,"Usage:%s portnumber\n",argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr,"Usage:%s portnumber\n",argv[0]);
        return 1;
    }
    //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件

    struct epoll_event ev,events[20];
    //生成用于处理accept的epoll专用的文件描述符

    epfd=epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //把socket设置为非阻塞方式
    //setnonblocking(listenfd);
    //设置与要处理的事件相关的文件描述符

    ev.data.fd=listenfd;
    //设置要处理的事件类型

    ev.events=EPOLLIN|EPOLLET;
    //ev.events=EPOLLIN;
    //注册epoll事件

    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    const char *local_addr="172.17.180.69";
    inet_aton(local_addr,&(serveraddr.sin_addr));//htons(portnumber);

    serveraddr.sin_port=htons(portnumber);
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
    while(1){
        nfds=epoll_wait(epfd,events,20,500);
        //处理所发生的所有事件

        for(i=0;i<nfds;++i)
        {
            if(events[i].data.fd==listenfd)//如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接
            {
                connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen);
                if(connfd<0){
                    perror("connfd<0");
                    exit(1);
                }
                //setnonblocking(connfd);

                char *str = inet_ntoa(clientaddr.sin_addr);
                cout << "accept a connection from " << str << endl;
                //设置用于读操作的文件描述符

                ev.data.fd=connfd;
                //设置用于注测的读操作事件

                ev.events=EPOLLIN|EPOLLET;
                //ev.events=EPOLLIN;

                //注册ev
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
            }
            else if(events[i].events&EPOLLIN)//如果是已经连接的用户，并且收到数据，那么读入
            {
                if ((sockfd = events[i].data.fd) < 0)
                    continue;
                if ((readNum = read(sockfd, line, MAXSIZE)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                        std::cout<<"readline error"<<std::endl;
                }
                else if (readNum == 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                line[readNum] = '\0';

                cout << "read = " << line << endl;
                string curVersion = parseOperation(line);
                cout << "v = " << curVersion << endl;
                if(curVersion == "")
                	continue;
                //生成diff_str
                string LatestVersion = "ver1002";
                UINT DIRLEN=256;
                CHAR olddir[DIRLEN];
                CHAR newdir[DIRLEN];
                snprintf(olddir,DIRLEN,"/home/hips/karlxu/workspace/BavUpdateServer/version_repo/");
                snprintf(newdir,DIRLEN,"/home/hips/karlxu/workspace/BavUpdateServer/version_repo/");
            	strncat(olddir,curVersion.c_str(),DIRLEN-1);
            	strncat(newdir,LatestVersion.c_str(),DIRLEN-1);

            	FileDiff fd;
                printf("olddir = %s\n",olddir);
                printf("newdir = %s\n",newdir);
                fd.setOldPathPrefix(olddir);
                fd.setNewPathPrefix(newdir);
                INT ret1 = fd.listDir(olddir,0,OLD_VERSION);
                INT ret2 = fd.listDir(newdir,0,NEW_VERSION);
                if(BAV_OK!=ret1||BAV_OK!=ret2)
                {
                	printf("ERROR in generating diff_str!ret1=%d,ret2=%d\n",ret1,ret2);
                	continue;
                }
                std::string diff_str = fd.diffFile();

                memset(line,0,MAXSIZE);
                snprintf(line,MAXSIZE, "%s", diff_str.c_str());

                //设置用于写操作的文件描述符
                ev.data.fd=sockfd;
                //设置用于注测的写操作事件
                ev.events=EPOLLOUT|EPOLLET;
                //修改sockfd上要处理的事件为EPOLLOUT

                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);

            }
            else if(events[i].events&EPOLLOUT) // 如果有数据发送
            {
                sockfd = events[i].data.fd;
                writeNum = write(sockfd, line, strlen(line));
                //设置用于读操作的文件描述符
                cout << "writeNum = "<<writeNum<<", write = " << line << endl;
                ev.data.fd=sockfd;
                //设置用于注测的读操作事件

                ev.events=EPOLLIN|EPOLLET;
                //修改sockfd上要处理的事件为EPOLIN

                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            }
        }
    }
    return 0;
}
