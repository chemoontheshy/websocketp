#ifndef __WEBSOCKET_SERVER_H_
#define __WEBSOCKET_SERVER_H_

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_INTERNAL_
#define ASIO_HAS_STD_TYPE_TRAITS
#define ASIO_HAS_STD_SHARED_PTR 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ATOMIC
#define ASIO_HAS_STD_CHRONO 
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_ARRAY 
#define ASIO_HAS_STD_SYSTEM_ERROR

#include <memory>
#include <thread>
#include <list>
#include <mutex>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>


namespace vsnc
{
	class WebSocketServer
	{
		using srv_type = std::unique_ptr<websocketpp::server<websocketpp::config::asio>>;

	public:
		WebSocketServer(const uint16_t port);

		~WebSocketServer();

		void Send(const std::string& msg);
		void Worker();

	private:
		//void Worker();
		void OnOpen(websocketpp::connection_hdl hdl);
		void OnClose(websocketpp::connection_hdl hdl);
		void OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);

	private:
		srv_type m_upServer;
		std::thread m_iRunner;

		std::mutex m_iMutex;
		std::list<websocketpp::connection_hdl> m_lstHandler;

	};

}

#endif // !__WEBSOCKET_SERVER_H_
