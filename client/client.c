#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define PORT    5555
#define SERVER_IP "127.0.0.1"

typedef struct mathopt
{
	int oprate;
	float value1;
	float value2;
}mopt;

void createopt(struct mathopt *pMp)
{
	printf("please input operand one:");
	scanf("%f",&(pMp->value1));

	printf("please input operand two:");
	scanf("%f",&(pMp->value2));
}
void help()
{
  printf("==================================================================\n");
  printf("版本version 1.0\n");
  printf("作者author：冯赫龙fenghelong\n\n\n");
  printf("%-12s %-12s\n","Command","Fuction");
  printf("%-12s %-12s\n","add","Adder computing");
  printf("%-12s %-12s\n","minus","Minus computing");
  printf("%-12s %-12s\n","multiply","Multiply computing");
  printf("%-12s %-12s\n","divide","Divide computing");
  printf("%-12s %-12s\n","hlep","Print help information");
  printf("%-12s %-12s\n","quit","Exit the program");
  printf("==================================================================\n");
}
main()
{
	
	struct sockaddr_in serverAddr;
	int clientSocket;
	char sendbuf[200];
	char recvbuf[200];
	char command[20];
	if((clientSocket=socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror( "error: create socket!!!");
		return -1; 		
	}
	
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(PORT);
	serverAddr.sin_addr.s_addr=inet_addr(SERVER_IP);

	if(connect(clientSocket,( struct sockaddr * )&serverAddr,sizeof(serverAddr)) < 0)
	{
		perror("error: connect remote server !!!");
		exit(1);
	}	
	printf("infor: connect with destination host........[ok]\n");
	printf ("\n");
	printf ("\n");
	printf ("please enter help for more information\n");
	printf ("\n");
	printf ("\n");
	while(1)
	{
		printf("Input your World:>");
		scanf("%s",command);
		struct mathopt mp;
		if(strcmp(command,"add") == 0)
		{
			createopt(&mp);
			mp.oprate = 0;
		}
		else if(strcmp(command,"minus") == 0)
		{
			createopt(&mp);
			mp.oprate = 1;
		}
		else if(strcmp(command,"multiply") == 0)
		{
			createopt(&mp);
			mp.oprate = 2;
		}
		else if(strcmp(command,"divide") == 0)
		{
			while(1)
			{
				createopt(&mp);
				if(mp.value2 == 0)
				{
					printf("warning: Dividend can not be 0!!!!!\n");
				}
				else
				{
					break;
				}
			}
			mp.oprate = 3;
		}
		else if(strcmp(command,"help") == 0)
		{
			help();
			continue ;
		}
		else if(strcmp(command,"quit") == 0)
		{
			mp.oprate = 4;
		}
		else
		{
			printf("error: can't identify your input !!!\n");
			continue ;
		}

		send(clientSocket,(char *)&mp,sizeof(mp),0);
		if(mp.oprate == 4)
		{
			break;
		}
		recv(clientSocket,recvbuf,200,0);
		printf("the result is: %s\n",recvbuf);
	}
	close(clientSocket);
	return 0;
}
