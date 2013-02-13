#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>  
#include <sys/types.h>   
#include <stdio.h>   
#include <errno.h>   
#include <sys/ioctl.h>   
#include <net/if.h>   
#include <netinet/ip.h>   
#include <netinet/in.h>   
#include <string.h>   
#include <arpa/inet.h>   
#include <netinet/tcp.h>   
#include <pthread.h>     
#include <signal.h>   

#define SERVER_PORT 5555
#define MAX_NUM  4

typedef struct mathopt
{
	int oprate;
	float value1;
	float value2;
}mopt;

typedef struct threadatom
{
	int clientsocket; //±êŒÇ¿ÉÓÃÌ×œÓ×Ö
	int flag;// ±êŒÇflagÖµ
}tatom;

struct threadatom threadpoolatom[MAX_NUM];
pthread_t threadid[MAX_NUM];
void cleanthreadpool()
{
	int i;
	for(i = 0; i < MAX_NUM; i++)
	{
		threadpoolatom[i].clientsocket = 0;
		threadpoolatom[i].flag = 0;
		threadid[i] = 0;
	}
}
void showthreadstatus()
{
	int i;
	for( i = 0; i < MAX_NUM; i++)
	{
		if(i % 5 == 0)
		{
			printf("\n");
		}
		printf("[%d] [%d] [%s] |",threadid[i],threadpoolatom[i].clientsocket,threadpoolatom[i].flag>=1?"open":"close");
	}
}

void* processthread(void *para)
{
	int index = *((int *)para);	
	struct threadatom *pthread = &threadpoolatom[index];
	printf("thread start %d\n",index);
	while(1)
	{		
		if(threadpoolatom[index].flag)
		{
			printf("thread %d\n",index);
			char buffer[1024];
			int iDataNum ;
			iDataNum = recv(pthread->clientsocket,buffer,1024,0);
			int length = sizeof(struct mathopt);
			if(iDataNum < length)
			{
				perror("error: Recv data !!!");
				pthread->clientsocket = 0;
				pthread->flag = 0;
				continue ;
			}
			struct mathopt *pMp = (struct mathopt *)buffer;
			float result = 0;
			if(pMp->oprate == 0)
			{
				result = pMp->value1 + pMp->value2;
			}
			else if(pMp->oprate == 1)
			{
				result = pMp->value1 - pMp->value2;
			}
			else if(pMp->oprate == 2)
			{
				result = pMp->value1 * pMp->value2;
			}
			else if(pMp->oprate == 3)
			{
				result = pMp->value1 / pMp->value2;
			}
			else if(pMp->oprate == 4)
			{
				pthread->clientsocket = 0;
				pthread->flag = 0;
				continue;
			}

			char buf[100];
			sprintf(buf,"%f",result);
			printf("v1:%f v2:%f o:%d r:%f\n",pMp->value1,pMp->value2,pMp->oprate,result);
			int error = send(pthread->clientsocket,buf,sizeof(buf),0);
			if(error < 0)
			{
				close(pthread->clientsocket);
				pthread->clientsocket = 0;
				pthread->flag = 0;
				continue;
			}
		}
		else
		{
			printf("sleep thread :%d\n",index);
			sleep(1) ;
		}
	}	
}

main()
{
	int serverSocket;
	struct sockaddr_in server_addr;
	struct sockaddr_in clientAddr;
	int addr_len = sizeof(clientAddr);	

	if((serverSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror( "error: create server socket!!!");
		exit(1);
	}

	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family =AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(serverSocket,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
	{
		perror("error: bind address !!!!");
		exit(1);
	}

	if(listen(serverSocket,5)<0)
	{
		perror("error: listen !!!!");
		exit(1);
	}
	
	cleanthreadpool();

	int i;
	for( i = 0 ; i < MAX_NUM; i++)
	{
		pthread_t temp;
		int err = 0;
		printf("i = %d\n",i);
		err = pthread_create(&temp, NULL, processthread, (void *)&i);   
		if(err > 0)
			printf("[%d] start ok............\n",i);
		sleep(2);
	}

	printf("init threadpool status:\n");
	showthreadstatus();
	while(1)
	{		
		int clientsocket;
		printf("accetp conn.....\n");
		clientsocket = accept(serverSocket,(struct sockaddr *)&clientAddr,(socklen_t*)&addr_len);
		if(clientsocket < 0)
		{
			perror("error: accept client socket !!!");
			continue;
		}
		printf("find pool.....\n");
		for( i = 0 ; i < MAX_NUM; i++)
		{
			if(threadpoolatom[i].flag == 0)
			{
				threadpoolatom[i].flag = 1;
				threadpoolatom[i].clientsocket = clientsocket;
				break;
			}
		}
		printf("now thread pool status:\n");
		showthreadstatus();		
	}
	close(serverSocket);
}

