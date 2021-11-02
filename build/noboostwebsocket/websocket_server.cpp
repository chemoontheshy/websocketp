#include "websocket_server.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
using vsnc::vnet::Pubisher;

namespace vsnc
{
	namespace vnet
	{
		static websocketpp::server<websocketpp::config::asio> __server;
		// 线程
		static std::thread __runner;
		// 线程锁
		static std::mutex __mutex;
		// 处理事件语句权柄
		static std::list<websocketpp::connection_hdl> __list_handler;

		/// <summary>
		/// 当websocket有请求时
		/// </summary>
		/// <param name="hdl"> 处理事件语句权柄</param>
		/// <param name="msg">请求的内容</param>
		static void __on_message(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) noexcept;
		
		/// <summary>
		/// 当websocket有新的接入链接时
		/// </summary>
		/// <param name="hdl">处理事件语句权柄</param>
		static void __on_open(websocketpp::connection_hdl hdl) noexcept;

		/// <summary>
		/// 当有websocket断开链接时
		/// </summary>
		/// <param name="hdl">处理事件语句权柄</param>
		static void __on_close(websocketpp::connection_hdl hdl) noexcept;

	}
}

void vsnc::vnet::__on_message(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) noexcept
{
}

void vsnc::vnet::__on_open(websocketpp::connection_hdl hdl) noexcept
{
	std::lock_guard<std::mutex> lock(__mutex);
	auto it = std::find_if(__list_handler.begin(), __list_handler.end(), [hdl](std::weak_ptr<void> p)->bool {return (hdl.lock() == p.lock()); });
	if (it == __list_handler.end())
	{
		__list_handler.push_back(hdl);
		std::cout << "new connect" << std::endl;
	}
}

void vsnc::vnet::__on_close(websocketpp::connection_hdl hdl) noexcept
{
	std::lock_guard<std::mutex> lock(__mutex);
	__list_handler.remove_if([hdl](std::weak_ptr<void> p)->bool {return (hdl.lock() == p.lock()); });
}

vsnc::vnet::Pubisher::Pubisher(const uint16_t port)
{
	try {
		// set loging settings
		__server.set_access_channels(websocketpp::log::alevel::none);
		//__server.set_access_channels(websocketpp::log::alevel::all);
		//__server.clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize Asio
		__server.init_asio();

		// Register our mssage handler
		//__server.set_message_handler(std::bind(&__on_message,std::placeholders::_1, std::placeholders::_2));
		__server.set_open_handler(std::bind(&__on_open,std::placeholders::_1));
		__server.set_close_handler(std::bind(&__on_close, std::placeholders::_1));

		// Listen on port 9002
		__server.listen(port);

		// Start the server accept loop
		__server.start_accept();
	}
	catch (websocketpp::exception const& e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "other exception" << std::endl;
	}
	__runner = std::thread([]() {__server.run(); });

}

vsnc::vnet::Pubisher::~Pubisher()
{
	__server.stop_listening();
	for (auto& hdl : __list_handler)
	{
		__server.close(hdl, websocketpp::close::status::blank, "");
	}
	__server.stop();
	__runner.join();
}

bool vsnc::vnet::Pubisher::Pubish(const Data& data)
{
	std::string str = "test";
	for (auto& hdl : __list_handler) {
		__server.send(hdl,data.ptr,data.len, websocketpp::frame::opcode::BINARY);
	}
	std::cout << std::hex << *data.ptr << std::endl;
	return true;
}


