/*
** Copyright 2011 Carnegie Mellon University
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef XSOCKET_H
#define XSOCKET_H

#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ATTEMPTS 100 //Number of attempts at opening a socket 
#define MAXBUFLEN 2000 // Note that this limits the size of chunk we can receive

#define XSOCK_STREAM 0 // Reliable transport
#define XSOCK_DGRAM 1 // Unreliable transport

struct Netinfo{
    unsigned short port;
    char xid[MAXBUFLEN];
    char src_path[MAXBUFLEN];
    char dst_path[MAXBUFLEN];
    int nxt;
    int last;
    uint8_t hlim;
    char status[20];
    char protocol[20];
} ;

//Function list
extern int Xsendto(int sockfd,const void *buf, size_t len, int flags,char * dDAG, size_t dlen);
extern int Xrecvfrom(int sockfd,void *buf, size_t len, int flags,char * dDAG, size_t *dlen);
extern int Xsocket(int transport_type); // 0: Reliable transport, 1: Unreliable transport
extern int Xconnect(int sockfd, char* dest_DAG);
extern int Xbind(int sockfd, char* SID);
extern int Xclose(int sock);
extern int Xrecv(int sockfd, void *buf, size_t len, int flags);
//extern int Xsend(int sockfd,const void *buf, size_t len, int flags);
extern int Xsend(int sockfd,const void *buf, size_t len, int flags);
extern int XgetCID(int sockfd, char* dDAG, size_t dlen);
extern int XputCID(int sockfd, const void *buf, size_t len, int flags,char* sDAG, size_t dlen);
extern int Xaccept(int sockfd);
extern int Xgetsocketidlist(int sockfd, int *socket_list);
extern int Xgetsocketinfo(int sockfd1, int sockfd2, struct Netinfo *info);
extern void error(const char *msg);
extern void set_conf(const char *filename, const char *sectioname);
extern void print_conf();
#ifdef __cplusplus
}
#endif

#endif
