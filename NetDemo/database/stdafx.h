// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <vector>
#include <string>
using namespace std;

#include <tchar.h>
#include <stdlib.h>
#include <io.h>
#include <string>
#include "../SQLite3/sqlite3.h"

#ifdef _DEBUG
#pragma comment(lib,"../../output/database/Debug/sqlite3.lib")
#else
#pragma comment(lib,"../../output/database/Realease/sqlite3.lib")
#endif

#include "SqliteDataDefine.h"
#include "CommonFuntion.h"
#include "Sqlite.h"
extern string RIM_RTK_BSD_DB_FILE;