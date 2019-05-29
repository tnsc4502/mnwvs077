#include "Employee.h"
#include "MiniRoomBase.h"
#include "..\WvsLib\Net\PacketFlags\EmployeePacketFlags.hpp"
#include "..\WvsLib\Net\OutPacket.h"

Employee::Employee()
{
}

Employee::~Employee()
{
}

void Employee::EncodeInitData(OutPacket * oPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	oPacket->Encode4(m_nEmployerID);
	oPacket->Encode4(m_pMiniRoom->GetEmployeeTemplateID()); //EmployeeTemplateID
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode2(GetFh());
	oPacket->EncodeStr(m_sEmployerName);
	oPacket->Encode1(m_pMiniRoom->GetType());
	oPacket->Encode4(m_pMiniRoom->GetMiniRoomSN());
	oPacket->EncodeStr(m_pMiniRoom->GetTitle());
	oPacket->Encode1(m_pMiniRoom->GetMiniRoomSpec());
	oPacket->Encode1(m_pMiniRoom->GetCurUsers());
	oPacket->Encode1(m_pMiniRoom->GetMaxUsers());
}

void Employee::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(EmployeeSendPacketFlag::Employee_OnMakeEnterFieldPacket);
	EncodeInitData(oPacket);
}

void Employee::MakeLeaveFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(EmployeeSendPacketFlag::Employee_OnMakeLeaveFieldPacket);
	oPacket->Encode4(m_nEmployerID);
}
