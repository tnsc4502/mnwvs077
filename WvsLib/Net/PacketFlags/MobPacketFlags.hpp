#pragma once
#ifndef MOB_FLAG
#define MOB_FLAG

#include "PacketFlags.hpp"

#define MAKE_MOB_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MobSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_MOB_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MobRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(MobSendPacketFlag)
//以下開始註冊封包Opcode Flag
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMakeEnterFieldPacket, 157);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMakeLeaveFieldPacket, 158);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobChangeController, 159);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMove, 160);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnCtrlAck, 161);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnStatSet, 163);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnStatReset, 164);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnSuspendReset, 165);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnAffected, 166);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnDamaged, 167);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnSpecialEffectBySkill, 168);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnHPIndicator, 170);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnCatchEffect, 171);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnEffectByItem, 172);
MAKE_MOB_SEND_PACKET_FLAG(Mob_OnMobAttackedByMob, 173);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END



MAKE_FLAG_COLLECTION_BODY(MobRecvPacketFlag)
//以下開始註冊封包Opcode Flag

MAKE_MOB_RECV_PACKET_FLAG(Mob_OnMove, 0x8C); //Mob::GenerateMovePath
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnApplyControl, 0x8D); //Mob::ApplyControl
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDropPickUpRequest, 0x8E); //Mob::SendDropPickUpReques
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDamagedByMob, 0x8F); //Mob::SetDamagedByMob
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnUpdateTimeBomb, 0x90); //Mob::UpdateTimeBomb
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnCollisionEscort, 0x91); //Mob::SendCollisionEscort
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnRequestEscortPath, 0x92); //Mob::SendRequestEscortPath
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnEscortStopEndRequest, 0x93); //Mob::SendEscortStopEndRequest
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnAttack_Area, 0x94); //Mob::DoAttack_Area
/*MAKE_MOB_RECV_PACKET_FLAG(Mob_OnExplosionStart, 0x378); //Mob::SendExplosionStart
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnInitMobUpDownState, 0x379); //Mob::InitMobUpDownState
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnUpdateFixedMoveDir, 0x37A); //Mob::UpdateFixedMoveDir
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnUpdatePassiveSkill, 0x37B); //Mob::UpdatePassiveSkill
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnRequsetAfterDead, 0x37C); //Mob::SendRequsetAfterDead
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnDamageShareInfoUpdate, 0x37D); //Mob::DamageShareInfoUpdate
MAKE_MOB_RECV_PACKET_FLAG(Mob_OnCreateAffectedArea, 0x37E); //Mob::CreateAffectedArea
*/

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif