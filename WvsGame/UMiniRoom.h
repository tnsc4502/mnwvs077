#pragma once

class User;
class InPacket;

class UMiniRoom
{
public:
	static void OnMiniRoom(User *pUser, InPacket *iPacket);
	static void OnMRCreate(User *pUser, InPacket *iPacket);
	static void OnMREnter(User *pUser, InPacket *iPacket);
	static void OnMRForward(User *pUser, int nType, InPacket *iPacket);
};

