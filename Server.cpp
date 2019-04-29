#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

#include <stdio.h>
#include <winsock2.h> // Wincosk2.h должен быть раньше windows!
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Server.h"

// макрос для печати количества активных пользователей
#define PRINTNUSERS if (nclients) printf("%d user on-line\n", nclients); \
         else printf("No User on line\n");
static int nclients = 0;
// Эта функция создается в отдельном потоке
// и обсуживает очередного подключившегося клиента независимо от остальных
DWORD WINAPI ToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET*)client_socket)[0];
	char buff[20 * 1024];
#define sHELLO "Fill DB with comand SEND then sort it with command SORT it and watch the result\r\n"
#define what_toAdd "print name to add\r\n"
#define no_data "nothing to sort, first use command SEND то add info to DB\r\n"
#define success "successfully added"

	// отправляем клиенту приветствие
	send(my_sock, sHELLO, sizeof(sHELLO), 0);
	std::vector<std::string> message;
	message = { "alex", "jack","brat","eagles" };
	std::sort(message.begin(), message.end());
	std::string result = "";
	int s = 0;
	std::string buff_s;
	for (int i = 0; i < message.size(); i++)
		result = result + " " + message[i];
	// цикл эхо-сервера: прием строки от клиента и возвращение ее клиенту
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
		// ставим завершающий ноль в конце строки
		buff[bytes_recv] = 0;
		if (!strcmp(&buff[0], "sort\n"))//если команда sort получена
		{
			if (message.size > 1)//если есть что сортировать
			{
				//(this->*fn_map[REQUEST_SORT])();
			//Как тут вызвать метод по ключу
			}
			else
				send(my_sock, no_data, sizeof(no_data), 0);//вывод ошибки содежимого в message
			
		}
		
		else if (!strcmp(&buff[0], "send\n"))//если команда send
		{
			
			send(my_sock, what_toAdd, sizeof(what_toAdd),0);
		}
		else
				//(this->*fn_map[REQUEST_SEND])();
							//Как тут вызвать метод по ключу
			
		//buff_s.clear();
				send(my_sock, success, sizeof(success),0);
	}
	//send(my_sock, &buff[0], bytes_recv, 0);

// если мы здесь, то произошел выход из цикла по причине
// возращения функцией recv ошибки - соединение с клиентом разорвано
	nclients--; // уменьшаем счетчик активных клиентов
	printf("-disconnect\n"); PRINTNUSERS

		// закрываем сокет
		closesocket(my_sock);
	return 0;
}

Server::Server()
{
	this->fn_map[REQUEST_SORT] = &Server::sort; //а теперь строка 1=ключ 1 и его значение это метод sort?
	this->fn_map[REQUEST_SEND] = &Server::send_name;
}

Server::Server(int port) : Base(port)
{
	this->fn_map[REQUEST_SORT] = &Server::sort; //а теперь строка 1=ключ 1 и его значение это метод sort?
	this->fn_map[REQUEST_SEND] = &Server::send_name;
}

int Server::init()
{
	printf("TCP SERVER DEMO\n");
	// Шаг 1 - Инициализация Библиотеки Сокетов
	// т.к. возвращенная функцией информация не используется
	// ей передается указатель на рабочий буфер, преобразуемый к указателю
	// на структуру WSADATA.
	// Такой прием позволяет сэкономить одну переменную, однако, буфер
	// должен быть не менее полкилобайта размером (структура WSADATA
	// занимает 400 байт)
	if (WSAStartup(MAKEWORD(2, 2), (WSADATA*)& buff[0]))
	{
		// Ошибка!
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	// AF_INET - сокет Интернета
	// SOCK_STREAM - потоковый сокет (с установкой соединения)
	// 0 - по умолчанию выбирается TCP протокол
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// Ошибка!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup(); // Деиницилизация библиотеки Winsock
		return -1;
	}

	// Шаг 3 - связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port); // не забываем о сетевом порядке!!!
	local_addr.sin_addr.s_addr = 0; // сервер принимает подключения
									// на все свои IP-адреса

	// вызываем bind для связывания
	if (bind(mysocket, (sockaddr*)& local_addr, sizeof(local_addr)))
	{
		// Ошибка
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket); // закрываем сокет!
		WSACleanup();
		return -1;
	}

	return 0;
}
int Server::handle()
{
	// Шаг 4 - ожидание подключений
	// размер очереди - 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("Ожидание подключений...\n");

	// Шаг 5 - извлекаем сообщение из очереди
	SOCKET client_socket; // сокет для клиента
	sockaddr_in client_addr; // адрес клиента (заполняется системой)

	// функции accept необходимо передать размер структуры
	int client_addr_size = sizeof(client_addr);

	// цикл извлечения запросов на подключение из очереди
	while ((client_socket = accept(mysocket, (sockaddr*)& client_addr, \
		& client_addr_size)))
	{
		nclients++; // увеличиваем счетчик подключившихся клиентов

		// пытаемся получить имя хоста
		HOSTENT* hst;
		hst = gethostbyaddr((char*)& client_addr.sin_addr.s_addr, 4, AF_INET);

		// вывод сведений о клиенте
		printf("+%s [%s] new connect!\n",
			(hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
		PRINTNUSERS

			// Вызов нового потока для обслужвания клиента
			// Да, для этого рекомендуется использовать _beginthreadex
			// но, поскольку никаких вызовов функций стандартной Си библиотеки
			// поток не делает, можно обойтись и CreateThread
			DWORD thID;
		CreateThread(NULL, NULL, ToClient, &client_socket, NULL, &thID);
	}
	return 0;
}

void Server::sort(std::vector<std::string> a) //вот и метод сорт который возвращает строку
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
void Server::send_name(std::string a) // метод send который дает команду записать в вектор messages новую строку
{
	message.push_back(a);
}



