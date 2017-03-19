#pragma once


/***************************************Server*******************************************/
bool				startServer(char *ip, int port);
bool				stopServer();
bool				isServerStoped();
int					cntServerConnections();
const char*			curConnectionsStr();

/***************************************Client*******************************************/
bool				startClient(char *ip, int port, bool bDevice);
bool				stopClient();
bool				isClientStoped();



/*****************************************************************************************/
typedef				void(__cdecl *QueryTableCallBack)(
					char*	  					dataStr,
					char*	  					errorStr
					);
void				queryTable(char* QuerySql, QueryTableCallBack callBack, bool bSync);
void				queryOnlieDevCnt(QueryTableCallBack callBack, bool bSync);


typedef				void(__cdecl *AddDataCallBack)(
	char*			strError
	);
typedef				AddDataCallBack ExcuteSqlCallBack;

void				addTable(char* tableName, char* dataStr, AddDataCallBack callBack, bool bSync);
void				excuteSql(char* sqlStr, ExcuteSqlCallBack callBack, bool bSync);
void				queryDevSpeed(char* ipStr, QueryTableCallBack callBack, bool bSync);
void				updateClientStatus(bool bNormal, bool bSync);
void				queryConnectionsStr(QueryTableCallBack callBack, bool bSync);