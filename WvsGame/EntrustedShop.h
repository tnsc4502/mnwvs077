#pragma once
#include "PersonalShop.h"
#include <string>

class User;

class EntrustedShop : public PersonalShop
{
	struct SoldItem
	{
		int nItemID = 0,
			nNumber = 0,
			nPrice = 0;

		std::string m_sBuyer;
	};

	enum EntrustedShopResult
	{
		res_EShop_ArrangeItem = 36,
		res_EShop_WithdrawAll = 38,
		res_EShop_WithdrawMoney = 40,
	};

	enum EntrustedShopRequest
	{
		req_EShop_PutItem = 0x1D,
		req_EShop_BuyItem = 0x1E,
		req_EShop_MoveItemToInventory = 0x22,
		req_EShop_GoOut = 0x23,
		req_EShop_ArrangeItem = 0x24,
		req_EShop_WithdrawAll = 0x25,
		req_EShop_WithdrawMoney = 0x27
	};

	int m_nEmployeeTemplateID = 0,
		m_nEmployerID = 0,
		m_nFieldID = 0;

	unsigned int m_tOpen = 0;
	long long int m_liEShopMoney = 0;
	bool m_bOnCreate = false;

	std::string m_sEmployerName;
	std::vector<SoldItem> m_aSoldItem;

public:
	EntrustedShop();
	~EntrustedShop();

	bool IsEmployer(User *pUser) const; 
	void OnPacket(User *pUser, int nType, InPacket *iPacket);
	void DoTransaction(User *pUser, int nSlot, Item* psItem, int nNumber);
	void OnGoOut(User *pUser, InPacket *iPacket);
	void OnArrangeItem(User *pUser, InPacket *iPacket);
	void OnWithdrawAll(User *pUser, InPacket *iPacket);
	bool OnWithdrawMoney(User *pUser, InPacket *iPacket, bool bSend);
	int GetLeaveType() const;
	int IsAdmitted(User *pUser, InPacket *iPacket, bool bOnCreate);
	virtual GW_ItemSlotBase* MoveItemToShop(GW_ItemSlotBase *pItem, User *pUser, int nTI, int nPOS, int nNumber, int *nPOS2);
	virtual bool RestoreItemFromShop(User *pUser, PersonalShop::Item* psItem);

	void OnLeave(User *pUser, int nLeaveType);
	void EncodeItemNumberChanged(OutPacket *oPacket, std::vector<Item*>& apItem);
	void SendItemBackup();
	void EncodeEnterResult(User *pUser, OutPacket *oPacket);
	void EncodeItemList(OutPacket *oPacket);
	void EncodeSoldItemList(OutPacket *oPacket);
	void Encode(OutPacket *oPacket);
	void Release();
	void RegisterOpenTime();
	void CloseShop();
	void OnOpened();
	void BroadcastItemList();

	int GetEmployeeTemplateID() const;
	void SetEmployeeTemplateID(int nTemplateID);
	int GetEmployerID() const;
	void SetEmployerID(int nEmployerID);
	int GetEmployeeFieldID() const;
	void SetEmployeeFieldID(int nFieldID);
	void SetEmployerName(const std::string& sName);
	const std::string& GetEmployerName() const;
};

