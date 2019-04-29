#include "Server.h"


int main()
{
	Server server(666);
	server.init();
	server.handle();

}