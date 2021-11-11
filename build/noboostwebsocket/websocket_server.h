#ifndef __WEBSOCKET_SERVER_H_
#define __WEBSOCKET_SERVER_H_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <list>
#include <mutex>

namespace vsnc
{
	namespace vnet
	{
		/// <summary>
		/// 数据结构
		/// </summary>
		struct Data
		{
			/// <summary>
			/// 指针头
			/// </summary>
			uint8_t* ptr;
			/// <summary>
			/// 数据长度
			/// </summary>
			size_t	 len;
		};

		class Pubisher
		{
		public:
			Pubisher(const uint16_t port);
			~Pubisher();

			bool Pubish(const Data& data);

		private:
			void Work() noexcept;

			void OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg);
			
			void OnOpen(websocketpp::connection_hdl hdl);

			void Onclose(websocketpp::connection_hdl hdl);

		private:
			websocketpp::server<websocketpp::config::asio> m_iServer;

			std::thread m_iRunner;

			std::mutex m_iMutex;

			std::list<websocketpp::connection_hdl> m_lstHandle;
		};
	}
}


#endif __WEBSOCKET_SERVER_H_