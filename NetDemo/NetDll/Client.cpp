#include "stdafx.h"
#include "Client.h"

client::client(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint &endpoint, OnReceiveCallBack pReceiveCallBack)
	: m_io_service(io_service)
	, m_pReceiveCallBack(pReceiveCallBack)
{
	m_pSession = session_ptr(new session(m_io_service, m_pReceiveCallBack));
	//we need to monitor for the client list change event ,a new client connects or one client gets disconnected,
	// and notify all clients when this happens.Thus,we need to keep an array of clients,

	m_pSession->socket().async_connect(endpoint,
		boost::bind(&client::handle_connect,
		this,
		m_pSession,
		boost::asio::placeholders::error));
}

void client::handle_connect(session_ptr new_session, const boost::system::error_code& ec)
{
	if (ec)
	{
		string str = string("\nhandle_connect:") + ec.message();
		OutputDebugStringA(str.c_str());
		return;
	}

	new_session->start();

	if (m_pReceiveCallBack)
		m_pReceiveCallBack((long)m_pSession.get(), NULL, 0, ec.value(), ec.message().c_str());
}

void client::run() {

	m_thread = boost::thread(boost::bind(&client::WorkThread, this));
}

void client::WorkThread()
{
	try
	{
		m_io_service.run();
	}
	catch (std::exception &e)
	{
		string str = string("\nIOCP:") + e.what();
		OutputDebugStringA(str.c_str());
	}
}

void client::stop()
{
	if (m_pSession != NULL)
		m_pSession->stop();
	m_io_service.stop();

	m_thread.join();
	m_pSession = NULL;
}

void client::send(BYTE* SendBuf, int dataLen)
{
	if (g_clientPtr != NULL && g_clientPtr->m_pSession != NULL && g_clientPtr->m_pSession->bstarted() == 1)
	{
		g_clientPtr->m_pSession->send(SendBuf, dataLen);
	}
}