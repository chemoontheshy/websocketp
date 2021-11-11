#include "websocket_server.h"
#include <Windows.h>

int main()
{
	vsnc::vnet::Pubisher server(9000);
	vsnc::vnet::Data data;
	char sd[5] = {1,2,3,4};
	data.ptr = reinterpret_cast<uint8_t*>(&sd);
	data.len = 4;
	while (1)
	{
		server.Pubish(data);
		Sleep(1000);
	}
	return 0;
}