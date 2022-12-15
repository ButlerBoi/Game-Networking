#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "GameManager.h"
#include "Scene.h"
#include "Character.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define EXIT "!exit"

int JoinServer() {
	int socketCreated = 0;
	struct sockaddr_in ipOfServer;
	socketCreated = socket(AF_INET, SOCK_STREAM, 0);
	memset(&ipOfServer, '0', sizeof(ipOfServer));
	ipOfServer.sin_family = AF_INET;

	if (inet_pton(AF_INET, "127.0.0.1", &ipOfServer.sin_addr) <= 0) {
		printf("Error: Wrong address");
		return -1;
	}
	ipOfServer.sin_port = htons(2017);

	if (connect(socketCreated, (struct sockaddr*)&ipOfServer, sizeof(ipOfServer)) < 0) {
		printf("Error: Connection Failed");
		return -1;
	}
	return socketCreated;
}

int main(int atgc, char** argv)
{

	GameManager* ptr = new GameManager();
	bool status = ptr->OnCreate();
	if (status == true) {
		ptr->Run();
	}
	else if (status == false) {
		std::cerr << "Fatal error occured. Cannot start this program" << std::endl;
	}

	constexpr int datasendSize = 256;
	char datasend[datasendSize];
	memset(datasend, '\0', datasendSize);

	int socketforClient = JoinServer();

	cout << "Player joined!" << endl;
	cout << "Fetching rnd seed from server..." << endl;

	//int sizeRead = read(socketforClient, datasend, datasendSize);

	long rndSeed;
	memcpy(&rndSeed, &datasend[0], sizeof(long));
	srand(rndSeed);

	WSADATA wsaData;
	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);

	string ipAdress = "127.0.0.1";

	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		system("pause");
		return 1;
	}

	sockaddr_in hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.sin_family = AF_UNSPEC;
	hints.sin_port = htons(54000);
	inet_pton(AF_INET, ipAdress.c_str(), &hints.sin_addr);
	

	int connResult = connect(ConnectSocket, (sockaddr*)&hints, sizeof(hints));
	if (connResult == SOCKET_ERROR) {
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	char buf[4096];
	string userInput;

	do {
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)	{
			int sendResult = send(ConnectSocket, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR){
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(ConnectSocket, buf, 4096, 0);
				if (bytesReceived > 0){
					cout << "Server: " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}

	} while (userInput.size() > 0);

	closesocket(ConnectSocket);
	WSACleanup();
	delete ptr;

	system("pause");
	return 0;
}
