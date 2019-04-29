#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

#include <stdio.h>
#include <winsock2.h> // Wincosk2.h ������ ���� ������ windows!
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Server.h"

// ������ ��� ������ ���������� �������� �������������
#define PRINTNUSERS if (nclients) printf("%d user on-line\n", nclients); \
         else printf("No User on line\n");
static int nclients = 0;
// ��� ������� ��������� � ��������� ������
// � ���������� ���������� ��������������� ������� ���������� �� ���������
DWORD WINAPI ToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET*)client_socket)[0];
	char buff[20 * 1024];
#define sHELLO "Fill DB with comand SEND then sort it with command SORT it and watch the result\r\n"
#define what_toAdd "print name to add\r\n"
#define no_data "nothing to sort, first use command SEND �� add info to DB\r\n"
#define success "successfully added"

	// ���������� ������� �����������
	send(my_sock, sHELLO, sizeof(sHELLO), 0);
	std::vector<std::string> message;
	message = { "alex", "jack","brat","eagles" };
	std::sort(message.begin(), message.end());
	std::string result = "";
	int s = 0;
	std::string buff_s;
	for (int i = 0; i < message.size(); i++)
		result = result + " " + message[i];
	// ���� ���-�������: ����� ������ �� ������� � ����������� �� �������
	int bytes_recv;

	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) &&
		bytes_recv != SOCKET_ERROR)
		//a = strlen(buff);
	{
		/*s = 0;
		for (int i = 0; buff[i] != '\n'; i++)
		{
			s += 1;
		}
		buff_s.append(buff, s);
		*/
		// ������ ����������� ���� � ����� ������
		buff[bytes_recv] = 0;
		if (!strcmp(&buff[0], "sort\n"))//���� ������� sort ��������
		{
			if (message.size > 1)//���� ���� ��� �����������
			{
				//(this->*fn_map[REQUEST_SORT])();
			//��� ��� ������� ����� �� �����
			}
			else
				send(my_sock, no_data, sizeof(no_data), 0);//����� ������ ���������� � message
			
		}
		
		else if (!strcmp(&buff[0], "send\n"))//���� ������� send
		{
			
			send(my_sock, what_toAdd, sizeof(what_toAdd),0);
		}
		else
				//(this->*fn_map[REQUEST_SEND])();
							//��� ��� ������� ����� �� �����
			
		//buff_s.clear();
				send(my_sock, success, sizeof(success),0);
	}
	//send(my_sock, &buff[0], bytes_recv, 0);

// ���� �� �����, �� ��������� ����� �� ����� �� �������
// ���������� �������� recv ������ - ���������� � �������� ���������
	nclients--; // ��������� ������� �������� ��������
	printf("-disconnect\n"); PRINTNUSERS

		// ��������� �����
		closesocket(my_sock);
	return 0;
}

Server::Server()
{
	this->fn_map[REQUEST_SORT] = &Server::sort; //� ������ ������ 1=���� 1 � ��� �������� ��� ����� sort?
	this->fn_map[REQUEST_SEND] = &Server::send_name;
}

Server::Server(int port) : Base(port)
{
	this->fn_map[REQUEST_SORT] = &Server::sort; //� ������ ������ 1=���� 1 � ��� �������� ��� ����� sort?
	this->fn_map[REQUEST_SEND] = &Server::send_name;
}

int Server::init()
{
	printf("TCP SERVER DEMO\n");
	// ��� 1 - ������������� ���������� �������
	// �.�. ������������ �������� ���������� �� ������������
	// �� ���������� ��������� �� ������� �����, ������������� � ���������
	// �� ��������� WSADATA.
	// ����� ����� ��������� ���������� ���� ����������, ������, �����
	// ������ ���� �� ����� ������������ �������� (��������� WSADATA
	// �������� 400 ����)
	if (WSAStartup(MAKEWORD(2, 2), (WSADATA*)& buff[0]))
	{
		// ������!
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return -1;
	}

	// ��� 2 - �������� ������
	// AF_INET - ����� ���������
	// SOCK_STREAM - ��������� ����� (� ���������� ����������)
	// 0 - �� ��������� ���������� TCP ��������
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// ������!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup(); // �������������� ���������� Winsock
		return -1;
	}

	// ��� 3 - ���������� ������ � ��������� �������
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port); // �� �������� � ������� �������!!!
	local_addr.sin_addr.s_addr = 0; // ������ ��������� �����������
									// �� ��� ���� IP-������

	// �������� bind ��� ����������
	if (bind(mysocket, (sockaddr*)& local_addr, sizeof(local_addr)))
	{
		// ������
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket); // ��������� �����!
		WSACleanup();
		return -1;
	}

	return 0;
}
int Server::handle()
{
	// ��� 4 - �������� �����������
	// ������ ������� - 0x100
	if (listen(mysocket, 0x100))
	{
		// ������
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("�������� �����������...\n");

	// ��� 5 - ��������� ��������� �� �������
	SOCKET client_socket; // ����� ��� �������
	sockaddr_in client_addr; // ����� ������� (����������� ��������)

	// ������� accept ���������� �������� ������ ���������
	int client_addr_size = sizeof(client_addr);

	// ���� ���������� �������� �� ����������� �� �������
	while ((client_socket = accept(mysocket, (sockaddr*)& client_addr, \
		& client_addr_size)))
	{
		nclients++; // ����������� ������� �������������� ��������

		// �������� �������� ��� �����
		HOSTENT* hst;
		hst = gethostbyaddr((char*)& client_addr.sin_addr.s_addr, 4, AF_INET);

		// ����� �������� � �������
		printf("+%s [%s] new connect!\n",
			(hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
		PRINTNUSERS

			// ����� ������ ������ ��� ����������� �������
			// ��, ��� ����� ������������� ������������ _beginthreadex
			// ��, ��������� ������� ������� ������� ����������� �� ����������
			// ����� �� ������, ����� �������� � CreateThread
			DWORD thID;
		CreateThread(NULL, NULL, ToClient, &client_socket, NULL, &thID);
	}
	return 0;
}

void Server::sort(std::vector<std::string> a) //��� � ����� ���� ������� ���������� ������
{
	std::sort(a.begin(), a.end());
	std::string result = "";
	int s = 0;
	//std::string buff_s;
	for (int i = 0; i < a.size(); i++)
		result = result + " " + a[i];
	strcpy(buff, result.c_str());
	//std::cout << buff;
	send(my_sock, &buff[0], result.length(), 0);
}
void Server::send_name(std::string a) // ����� send ������� ���� ������� �������� � ������ messages ����� ������
{
	message.push_back(a);
}



