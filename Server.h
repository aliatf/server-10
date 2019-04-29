#pragma once
#pragma once
#include <map>
#include <vector>
//#include "../Base/Header.h"
#include "Header.h"
class Server : Base
{
public:
	const std::string REQUEST_SORT = "sort"; //��������� ������ 1 
	const std::string REQUEST_SEND = "send"; //��������� ������ 2
	Server();
	Server(int port);
	int init();
	int handle();
	typedef void (Server::* CallbackFunc)(void); //��� � �������
	std::map<std::string, CallbackFunc> fn_map;
	void sort(std::vector<std::string> a);
	void send_name(std::string a);
	std::vector<std::string> message;
	std::string save_name;
private:
	char buff[1024];
	SOCKET mysocket;
};

