#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h> // Wincosk2.h должен быть раньше windows!
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

class Base
{
public:
	Base() {}
	Base(int _port) : port(_port) {}
	virtual int handle() = 0;
	int GetPort() const { return port; }
	int SetPort(int _port) { port = _port; }

protected:
	int port = 666;
};