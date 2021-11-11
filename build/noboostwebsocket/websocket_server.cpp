#include "websocket_server.h"
using vsnc::vnet::Pubisher;


vsnc::vnet::Pubisher::Pubisher(const uint16_t port)
{
	try {
		// set loging settings
		m_iServer.set_access_channels(websocketpp::log::alevel::none);
		//__server.set_access_channels(websocketpp::log::alevel::all);
		//__server.clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize Asio
		m_iServer.init_asio();

		// Register our mssage handler
		//__server.set_message_handler(std::bind(&__on_message,std::placeholders::_1, std::placeholders::_2));
		m_iServer.set_open_handler(std::bind(&Pubisher::OnOpen,this,std::placeholders::_1));
		m_iServer.set_close_handler(std::bind(&Pubisher::Onclose,this, std::placeholders::_1));

		// Listen on port 9002
		m_iServer.listen(port);

		// Start the server accept loop
		m_iServer.start_accept();
	}
	catch (websocketpp::exception const& e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "other exception" << std::endl;
	}
	m_iRunner = std::thread([this]() {this->Work(); });

}

vsnc::vnet::Pubisher::~Pubisher()
{
	m_iServer.stop_listening();
	for (auto& hdl : m_lstHandle)
	{
		m_iServer.close(hdl, websocketpp::close::status::blank, "");
	}
	m_iServer.stop();
	m_iRunner.join();
}

bool vsnc::vnet::Pubisher::Pubish(const Data& data)
{
	for (auto& hdl : m_lstHandle) {
		m_iServer.send(hdl,data.ptr,data.len, websocketpp::frame::opcode::BINARY);
	}
	return true;
}

void vsnc::vnet::Pubisher::Work() noexcept
{
	try {
		m_iServer.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "other exception" << std::endl;
	}
	
}

void vsnc::vnet::Pubisher::OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
}

void vsnc::vnet::Pubisher::OnOpen(websocketpp::connection_hdl hdl)
{
	std::lock_guard<std::mutex> lock(m_iMutex);
	auto it = std::find_if(m_lstHandle.begin(), m_lstHandle.begin(), [hdl](std::weak_ptr<void> p)->bool {return (hdl.lock() == p.lock()); });
	if (it == m_lstHandle.end())
	{
		m_lstHandle.push_back(hdl);
		std::cout << "new connect" << std::endl;
	}
}

void vsnc::vnet::Pubisher::Onclose(websocketpp::connection_hdl hdl)
{
	std::lock_guard<std::mutex> lock(m_iMutex);
	m_lstHandle.remove_if([hdl](std::weak_ptr<void> p)->bool {return (hdl.lock() == p.lock()); });
}


