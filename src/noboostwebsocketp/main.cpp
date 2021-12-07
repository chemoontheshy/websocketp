#include "websocket_server.h"
#ifdef _WIN32
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#endif // _Win32

#ifdef _WIN32
void init()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData)) {
		std::cout << "error" << std::endl;
		exit(0);
	}
}
#endif // _Win32

int main()
{
	//
	//vsnc::vnet::Pubisher pubisher(9000);
	vsnc::vnet::Pubisher* pubisher  = nullptr;
	//vsnc::vnet::Pubisher* pubisher1 = nullptr;
	//vsnc::vnet::Pubisher* pubisher2 = nullptr;
	//vsnc::vnet::Pubisher* pubisher3 = nullptr;
	pubisher = new vsnc::vnet::Pubisher(9000);
	//pubisher1 = new vsnc::vnet::Pubisher(9001);
	//pubisher2 = new vsnc::vnet::Pubisher(9002);
	//pubisher3 = new vsnc::vnet::Pubisher(9003);
	vsnc::vnet::Data data;
	//½ÓÊÕ
#ifdef _WIN32
	init();
#endif // _Win32
	auto socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4000);
	((struct sockaddr_in&)addr).sin_addr.s_addr = INADDR_ANY;
	if (bind(socketFD, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) <0) {
		std::cout << "UDP::bind() failed." << std::endl;
		return 0;
	}
	int num = 0;
	char rBuf[1500] = {0};
	sockaddr_in client;
	socklen_t clientLen = sizeof(client);

	while (true) {
		auto size = recvfrom(socketFD, rBuf, 1500, 0, reinterpret_cast<sockaddr*>(&client), &clientLen);
		if (size > 0) {
			//std::cout << size << std::endl;
			data.len = size;
			data.ptr = reinterpret_cast<uint8_t*>(&rBuf);
			pubisher->Pubish(data);
		}
	}
	return 0;
}
