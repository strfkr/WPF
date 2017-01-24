#pragma once

#include "stdafx.h"
#include "NetDll.h"

enum PackTypeEnum
{
	PackTypeEnum_heart								= 0,
	PackTypeEnum_normal								= 1,
};

#pragma pack(push, 1)
//��ͷ
typedef struct tagPACKAGEHEAD
{
	BYTE											head1;			//0xff
	BYTE											head2;			//0xfe
	BYTE											packType;		//������
	INT												len;			//������
}PACKAGEHEAD;

#pragma pack(pop)