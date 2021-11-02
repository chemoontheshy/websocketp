#ifndef __WEBSOCKET_SERVER_H_
#define __WEBSOCKET_SERVER_H_

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_INTERNAL_


#include <iostream>

namespace vsnc
{
	
	namespace vent
	{
		class WebSocketServer
		{

		public:
			WebSocketServer(const uint16_t port);

			~WebSocketServer();

			void Send(const std::string& msg);

		};
	}
	
}

#endif // !__WEBSOCKET_SERVER_H_
