#include "stdafx.h"
#include "Client.h"
#include "atlbase.h"
#include "atlstr.h"
CClient* CClient::m_pClient				= NULL;

CClient::CClient()
{
	HMODULE hNetDll						= LoadLibrary(_T("NetDll.dll"));

	m_startClientFunc					= (_startClientType)GetProcAddress(hNetDll, "startClient");
	m_stopClientFunc					= (_stopClientType)GetProcAddress(hNetDll, "stopClient");
	m_isClientStopedFunc				= (_isClientStoped)GetProcAddress(hNetDll, "isClientStoped");
	m_clientSendBufLenFunc				= (_clientSendBufLen)GetProcAddress(hNetDll, "clientSendBufLen");

	GetAvalibleIpAddress(m_ipList);
	for (int i = 0; i < m_ipList.size(); ++i)
	{
		if (m_ipList[i] != "127.0.0.1")
		{
			m_ipStr = m_ipList[i];
			break;
		}
	}
	m_globalPackNumber					= 0;
}


CClient::~CClient()
{
	StopClient();
}

void CClient::OnReceiveCallBackFunc(long userID, BYTE* buf, int len, int errorcode, const char* errormsg)
{

	if (buf != NULL && len > 0 && errorcode == 0)
	{
		netmsg::MsgPack msgPack;
		if (msgPack.ParseFromArray(buf, len))
		{
			pair<DWORD, void*> pairValue;
			DWORD key = msgPack.head().globalpacknumber();

			if (msgPack.has_querydevspeedmsg())
			{
				int msgLen			= msgPack.ByteSize();
				LPBYTE pBuffer		= new BYTE[msgLen];
				msgPack.SerializeToArray(pBuffer, msgLen);
				CClient::GetInstance()->m_sendDataFunc(pBuffer, msgLen);
				delete[] pBuffer;
				OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_querydevspeedmsg");
			}
			else if (msgPack.has_upgradedownloadmsg())
			{
				string resulterror = msgPack.upgradedownloadmsg().resulterror();

				TCHAR temppath[MAX_PATH] ={ 0 };
				GetTempPath(MAX_PATH, temppath);
				string setupFilePath = string(CT2A(temppath)) + "setup.exe";

				ofstream stream(setupFilePath, ios::out|ios::trunc|ios::binary);
				stream.write((char*)msgPack.upgradedownloadmsg().seteupdata().c_str(), msgPack.upgradedownloadmsg().seteupdata().size());
				stream.close();

				OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__, "client: client.receive：has_upgradedownloadmsg");
			}
			else if (msgPack.has_upgradedownloadmsgresult())
			{
				CClient::GetInstance()->m_requestMap.Query(key, pairValue);
				string resultstr = msgPack.upgradedownloadmsgresult().resultdata();
				string resulterror = msgPack.upgradedownloadmsgresult().resulterror();
				if (pairValue.second)
					((CallBack)pairValue.second)((char*)resultstr.c_str(), (char*)resulterror.c_str());
				OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__, "client: client.receive：has_upgradedownloadmsgresult");
			}
			else if (CClient::GetInstance()->m_requestMap.Query(key, pairValue))
			{
				if (msgPack.has_registtypemsgresult())
				{
					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_registtypemsgresult");
				}
				else if (msgPack.has_querymsgresult())
				{
					string resultstr = msgPack.querymsgresult().resultdata();
					string resulterror = msgPack.querymsgresult().resulterror();
					if (pairValue.second)
						((CallBack)pairValue.second)((char*)resultstr.c_str(), (char*)resulterror.c_str());
					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_querymsgresult");
				}
				else if (msgPack.has_addmsgresult())
				{
					if (pairValue.second)
						((CallBack)pairValue.second)("", (char*)msgPack.addmsgresult().resulterror().c_str());
					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_addmsgresult");
				}
				else if (msgPack.has_querydevcntmsgresult())
				{
					int cnt = msgPack.querydevcntmsgresult().devcnt();
					char ch[MAX_PATH] = {0};
					sprintf_s(ch, "count:%d,;", cnt);
					if (pairValue.second)
						((CallBack)pairValue.second)(ch, "");
					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_querydevcntmsgresult");
				}
				else if (msgPack.has_excutesqlmsgresult())
				{
					string resulterror = msgPack.excutesqlmsgresult().resulterror();
					if (pairValue.second)
						((CallBack)pairValue.second)("",(char*)resulterror.c_str());
					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_excutesqlmsgresult");
				}
				else if (msgPack.has_querydevspeedmsgresult())
				{
					int speed = msgPack.querydevspeedmsgresult().speed();
					string resulterror = msgPack.querydevspeedmsgresult().resulterror();

					char ch[MAX_PATH] ={ 0 };
					sprintf_s(ch, "speed:%d,;", speed);
					if (pairValue.second)
						((CallBack)pairValue.second)(ch, (char*)resulterror.c_str());
					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_querydevspeedmsgresult");
				}
				else if (msgPack.has_queryconnectionsstrmsgresult())
				{
					string resultstr	= msgPack.queryconnectionsstrmsgresult().resultdata();
					string resulterror	= msgPack.queryconnectionsstrmsgresult().resulterror();

					if (pairValue.second)
						((CallBack)pairValue.second)((char*)resultstr.c_str(), (char*)resulterror.c_str());

					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.receive：has_queryconnectionsstrmsgresult");
				}
				else if (msgPack.has_addversionmsgresult())
				{
					string resultstr	= msgPack.addversionmsgresult().resultdata();
					string resulterror	= msgPack.addversionmsgresult().resulterror();

					if (pairValue.second)
						((CallBack)pairValue.second)((char*)resultstr.c_str(), (char*)resulterror.c_str());

					OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__, "client: client.receive：has_addversionmsgresult");
				}
				CClient::GetInstance()->m_requestMap.Pop(key, pairValue);
			}
		}
		else
		{
			OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__," Work.dll错误数据包");
		}
	}
	else
	{
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__," Work.dll没有数据");
	}
}

CClient* CClient::GetInstance()
{
	if (m_pClient == NULL)
		m_pClient = new CClient();

	return m_pClient;
}

void CClient::ReleaseInstance()
{
	if (m_pClient)
	{
		delete m_pClient;
		m_pClient = NULL;
	}
}

bool CClient::StartClient(char *ip, int port, bool bDevice)
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_startClientFunc && m_startClientFunc(ip, port, CClient::OnReceiveCallBackFunc, m_sendDataFunc))
	{
		m_bDevice = bDevice;
		UpdateClientStatus(true, false);
		return true;
	}

	return false;
}

bool CClient::StopClient()
{
	// TODO:  在此添加控件通知处理程序代码

	bool bRet = false;
	if (m_stopClientFunc)
		bRet = m_stopClientFunc();

	m_bDevice			= false;
	m_globalPackNumber	= 0;
	m_requestMap.Clear();

	return bRet;
}

bool CClient::ClientStoped()
{
	if (m_isClientStopedFunc)
		return m_isClientStopedFunc();

	return false;
}

void CClient::ReportProgress(int packLen, CallBack callBack)
{
	if (m_clientSendBufLenFunc && callBack)
	{
		int len = m_clientSendBufLenFunc();

		char ch[MAX_PATH] = { 0 };
		sprintf_s(ch, MAX_PATH, "progress:%d,;", int((packLen - len) * 100.0 / packLen));
		callBack(ch, "");
	}
}

void CClient::UpdateClientStatus(bool bNormal, bool bWait)
{
	if (m_sendDataFunc)
	{
		DWORD numberTemp				= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, nullptr);
		netmsg::MsgPack pack;
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_registtype()->set_bdevice(m_bDevice);
		pack.mutable_registtype()->set_ip(m_ipStr);
		pack.mutable_registtype()->set_serverip("");
		pack.mutable_registtype()->set_bnormal(bNormal);

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_registtype");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

void CClient::QueryOnlieDevCnt(CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		DWORD numberTemp				= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		netmsg::MsgPack pack;
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_querydevcntmsg();

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_querydevcntmsg");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

void CClient::QueryTable(char* QuerySql, CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		DWORD numberTemp				= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		netmsg::MsgPack pack;
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_query()->set_msg(QuerySql);

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_query");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

void CClient::AddTable(char* tableName, char* dataStr, CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		netmsg::MsgPack pack;
		DWORD numberTemp			= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_add()->set_tablename(tableName);
		pack.mutable_add()->set_msg(dataStr);

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_add");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}
void CClient::AddVersion(char* Bianhao, char* Banbenhao, LPBYTE Anzhuangbao, int datalen, CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		netmsg::MsgPack pack;
		DWORD numberTemp			= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_addversionmsg()->set_bianhao(Bianhao);
		pack.mutable_addversionmsg()->set_banbenhao(Banbenhao);
		pack.mutable_addversionmsg()->set_anzhuangbao(Anzhuangbao, datalen);

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__, "client: client.send：mutable_add");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				ReportProgress(msgLen, callBack);
				Sleep(10);
			}
		}
	}
}
void CClient::ExcuteSql(char* sqlStr, CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		netmsg::MsgPack pack;
		DWORD numberTemp			= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_excutesqlmsg()->set_msg(sqlStr);

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_excutesqlmsg");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

void  CClient::QueryDevSpeed(char* ipStr, CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		netmsg::MsgPack pack;
		DWORD numberTemp			= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_querydevspeedmsg()->set_ipstr(ipStr);
		pack.mutable_querydevspeedmsg()->set_askuserid(-1);
		pack.mutable_querydevspeedmsg()->set_starttime(-1);

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_querydevspeedmsg");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

void  CClient::QueryConnectionsStr(CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		netmsg::MsgPack pack;
		DWORD numberTemp			= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		pack.mutable_queryconnectionsstrmsg();

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__,"client: client.send：mutable_queryconnectionsstrmsg");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

void  CClient::Upgrade(char* dataStr, CallBack callBack, bool bWait)
{
	if (m_sendDataFunc)
	{
		netmsg::MsgPack pack;
		DWORD numberTemp			= InterlockedIncrement(&m_globalPackNumber);
		m_requestMap.Push(numberTemp, callBack);
		pack.mutable_head()->set_globalpacknumber(numberTemp);
		pack.mutable_head()->set_totalpack(1);
		pack.mutable_head()->set_packindex(0);

		vector<string> rows; rows.reserve(1000);
		vector<string> cells; cells.reserve(1000);
		vector<string> keyvalue;
		split(dataStr, rows, ";");
		for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
		{
			string& row = rows[rowIndex];
			if (row.length() > 0)
			{
				cells.clear();
				split(row, cells, ",");
				for (int colIndex = 0; colIndex < cells.size(); ++colIndex){
					string& cell = cells[colIndex];
					keyvalue.clear();
					split(cell, keyvalue, ":");
					if (keyvalue.size() != 2)
						continue;
					if (keyvalue[0] == "version")
						pack.mutable_upgrademsg()->set_version(keyvalue[1]);
					else if (keyvalue[0] == "ip")
						pack.mutable_upgrademsg()->set_ip(keyvalue[1]);
				}
			}
		}

		int msgLen			= pack.ByteSize();
		LPBYTE pBuffer		= new BYTE[msgLen];
		pack.SerializeToArray(pBuffer, msgLen);

		m_sendDataFunc(pBuffer, msgLen);
		delete[] pBuffer;
		OutDebugLineLogs(__FILE__, __LINE__, __FUNCTION__, "client: client.send：mutable_upgrademsg");

		if (bWait)
		{
			MSG msg;
			while (m_requestMap.Exist(numberTemp))
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				Sleep(100);
			}
		}
	}
}

