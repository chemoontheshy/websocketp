#include "websocket_server.h"
using vsnc::WebSocketServer;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

vsnc::WebSocketServer::WebSocketServer(const uint16_t port):
    m_upServer(std::make_unique<websocketpp::server<websocketpp::config::asio>>())
{
    try {
        // Set logging settings
        m_upServer->set_access_channels(websocketpp::log::alevel::none);
        // m_upServer->set_access_channels(websocketpp::log::alevel::all);
        // m_upServer->clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        m_upServer->init_asio();

        // Register our message handler
        m_upServer->set_message_handler(std::bind(&WebSocketServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
        m_upServer->set_open_handler(std::bind(&WebSocketServer::OnOpen, this, std::placeholders::_1));
        m_upServer->set_close_handler(std::bind(&WebSocketServer::OnClose, this, std::placeholders::_1));
        // Listen on port 9002
        m_upServer->listen(port);

        // Start the server accept loop
        m_upServer->start_accept();

        // Start the ASIO io_service run loop
        //echo_server.run();
    }
    catch (websocketpp::exception const& e) {
        std::cout << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "other exception" << std::endl;
    }
    m_iRunner = std::thread([this]() {this->Worker(); });

   
}

WebSocketServer::~WebSocketServer()
{
    m_upServer->stop_listening();
    for (auto& hdl : m_lstHandler)
    {
        m_upServer->close(hdl, websocketpp::close::status::blank, "");
    }
    m_upServer->stop();
    m_iRunner.join();
}

void WebSocketServer::Send(const std::string& msg)
{
    for (auto& hdl : m_lstHandler) {
        m_upServer->send(hdl, msg,websocketpp::frame::opcode::text);
    }
}

void WebSocketServer::Worker()
{
    try
    {
        m_upServer->run();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "other exception" << std::endl;
    }

}

void WebSocketServer::OnOpen(websocketpp::connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_iMutex);
    auto it = std::find_if(m_lstHandler.begin(), m_lstHandler.end(), [hdl](std::weak_ptr<void> p)->bool {return (hdl.lock() == p.lock()); });
    if (it == m_lstHandler.end()) {
        m_lstHandler.push_back(hdl);
        std::cout << "new connect" << std::endl;
    }
}

void WebSocketServer::OnClose(websocketpp::connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_iMutex);
    m_lstHandler.remove_if([hdl](std::weak_ptr<void> p)->bool {return (hdl.lock() == p.lock()); });

}

void WebSocketServer::OnMessage(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
    std::cout << "on_message called with hdl: " << hdl.lock().get()
        << " and message: " << msg->get_payload()
        << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        m_upServer->stop_listening();
        return;
    }

    try {
        m_upServer->send(hdl, msg->get_payload(), msg->get_opcode());
        std::cout <<"opcode: "<<msg->get_opcode() << std::endl;
    }
    catch (websocketpp::exception const& e) {
        std::cout << "Echo failed because: "
            << "(" << e.what() << ")" << std::endl;
    }
}
