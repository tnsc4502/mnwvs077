#pragma once

class User;
class InPacket;

class UMiniRoom
{
public:
	static void OnMiniRoom(User *pUser, InPacket *iPacket);
	static void OnMRCreate(User *pUser, InPacket *iPacket);
	void OnMRForward(User *pUser, InPacket *iPacket);
};

