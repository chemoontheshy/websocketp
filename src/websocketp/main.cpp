#include "websocket_server.h"
#include <Windows.h>

int main()
{
	vsnc::WebSocketServer websocket(9000);
	
	std::string data = "test";
	while (1)
	{
		websocket.Send(data);
		Sleep(1000);
	}
	return 0;
}
