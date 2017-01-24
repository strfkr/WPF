#pragma once
#include "stdafx.h"
#pragma once

#include "Session.h"

class server
{
public:
	server(boost::asio::io_service &io_service, boost::asio::ip::tcp::endpoint &endpoint, OnReceiveCallBack pReceiveCallBack);
private:
	boost::asio::io_service &						m_io_service;
	boost::asio::ip::tcp::acceptor					m_acceptor;
	boost::thread_group								m_tg;
	OnReceiveCallBack								m_pReceiveCallBack;


	void											handle_accept(session_ptr new_session, const boost::system::error_code& error);
	void											WorkThread();
public:
	static boost::mutex								m_sessionsMutex;
	static list<session_ptr>						m_sessions;
	static void										send(long userID, BYTE* SendBuf, int dataLen);
	void											run();
	void											stop();
};