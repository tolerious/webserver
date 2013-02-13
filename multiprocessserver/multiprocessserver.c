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

typedef struct mathopt
{
	int oprate;
	float value1;
	float value2;
}mopt;

void processchild(int clientsocket)
{
	char buffer[1024];
	int iDataNum ;

	while(1)
	{
		iDataNum = recv(clientsocket,buffer,1024,0);
		int length = sizeof(struct mathopt);
		if(iDataNum < length)
		{
			perror("error: Recv data !!!");
			break ;
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
			break;
		}
		char buf[100];
		sprintf(buf,"%f",result);
		printf("v1:%f v2:%f o:%d r:%f\n",pMp->value1,pMp->value2,pMp->oprate,result);
		int error = send(clientsocket,buf,sizeof(buf),0);
		if(error < 0)
			break;
	}
	close(clientsocket);
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

	int optval = SO_REUSEADDR;
	setsockopt(serverSocket,SOL_SOCKET,optval,&optval,sizeof(optval));
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

	while(1)
	{		
		int clientsocket;
		clientsocket = accept(serverSocket,(struct sockaddr *)&clientAddr,(socklen_t*)&addr_len);
		if(clientsocket < 0)
		{
			perror("error: accept client socket !!!");
			continue;
		}
		
		int pid = fork();
		if(pid > 0)
		{
			close(clientsocket);
			continue;
		}
		else if(pid == 0)
		{
			close(serverSocket);
			processchild(clientsocket);
			exit(0);
		}
		else
		{
			printf("error: fork child process !!!!");
			exit(0);
		}
	}
	close(serverSocket);
}

