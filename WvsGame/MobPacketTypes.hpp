#pragma once
#ifndef MOB_FLAG
#define MOB_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(MobSendPacketType)
//以下開始註冊封包Opcode Flag
	REGISTER_TYPE(Mob_OnMakeEnterFieldPacket, 157);
	REGISTER_TYPE(Mob_OnMakeLeaveFieldPacket, 158);
	REGISTER_TYPE(Mob_OnMobChangeController, 159);
	REGISTER_TYPE(Mob_OnMove, 160);
	REGISTER_TYPE(Mob_OnCtrlAck, 161);
	REGISTER_TYPE(Mob_OnStatSet, 163);
	REGISTER_TYPE(Mob_OnStatReset, 164);
	REGISTER_TYPE(Mob_OnSuspendReset, 165);
	REGISTER_TYPE(Mob_OnAffected, 166);
	REGISTER_TYPE(Mob_OnDamaged, 167);
	REGISTER_TYPE(Mob_OnSpecialEffectBySkill, 168);
	REGISTER_TYPE(Mob_OnHPIndicator, 170);
	REGISTER_TYPE(Mob_OnCatchEffect, 171);
	REGISTER_TYPE(Mob_OnEffectByItem, 172);
	REGISTER_TYPE(Mob_OnMobAttackedByMob, 173);

//結束Opcode Flag註冊
END_PACKET_TYPE(MobSendPacketType)



DEFINE_PACKET_TYPE(MobRecvPacketType)
//以下開始註冊封包Opcode Flag

	REGISTER_TYPE(Mob_OnMove, 0x8C); //Mob::GenerateMovePath
	REGISTER_TYPE(Mob_OnApplyControl, 0x8D); //Mob::ApplyControl
	REGISTER_TYPE(Mob_OnDropPickUpRequest, 0x8E); //Mob::SendDropPickUpReques
	REGISTER_TYPE(Mob_OnDamagedByMob, 0x8F); //Mob::SetDamagedByMob
	REGISTER_TYPE(Mob_OnUpdateTimeBomb, 0x90); //Mob::UpdateTimeBomb
	REGISTER_TYPE(Mob_OnCollisionEscort, 0x91); //Mob::SendCollisionEscort
	REGISTER_TYPE(Mob_OnRequestEscortPath, 0x92); //Mob::SendRequestEscortPath
	REGISTER_TYPE(Mob_OnEscortStopEndRequest, 0x93); //Mob::SendEscortStopEndRequest
	REGISTER_TYPE(Mob_OnAttack_Area, 0x94); //Mob::DoAttack_Area
/*	REGISTER_TYPE(Mob_OnExplosionStart, 0x378); //Mob::SendExplosionStart
	REGISTER_TYPE(Mob_OnInitMobUpDownState, 0x379); //Mob::InitMobUpDownState
	REGISTER_TYPE(Mob_OnUpdateFixedMoveDir, 0x37A); //Mob::UpdateFixedMoveDir
	REGISTER_TYPE(Mob_OnUpdatePassiveSkill, 0x37B); //Mob::UpdatePassiveSkill
	REGISTER_TYPE(Mob_OnRequsetAfterDead, 0x37C); //Mob::SendRequsetAfterDead
	REGISTER_TYPE(Mob_OnDamageShareInfoUpdate, 0x37D); //Mob::DamageShareInfoUpdate
	REGISTER_TYPE(Mob_OnCreateAffectedArea, 0x37E); //Mob::CreateAffectedArea
*/

//結束Opcode Flag註冊
END_PACKET_TYPE(MobRecvPacketType)

#endif