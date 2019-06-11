
require "./DataSrv/Script/sysDef"

-- // Getting the Ticket
function s_get_ticket() {
	cTime = System.getDateTime();
	field = self->getField();
    inven = inventory;
    state = field->getContiState();
    boardingTime = field->getContiBoardingTime();
	-- // The ride from Orbis to Ludibrium : 10 minute interval or Ludibrium to Orbis : 10 minute interval
	if ( field->getID() == 200000121 or field->getID() == 220000110 ) {

		if ( state != 1 ) self->say( "目前飛船尚未抵達，請稍後片刻。飛船抵達時約需要1至5分鐘的準備時間，請耐心等候，謝謝。");
		else if ( boardingTime > cTime ) self->say( "目前飛船停泊中，稍後就會陸續開始檢驗傳票。請耐心等待約1至5分鐘，謝謝您的配合。");
		else {
            wField = field->getID();
            if ( target->getLevel() >= 30 ) {
                if ( field->getID() == 200000121 ) {
                    ticketID = 4031074;
                    wField = 200000122;
                }
                else if ( field->getID() == 220000110 ) {
                    ticketID = 4031045;
                    wField = 220000111;
                }
            }
            else if ( target->getLevel() < 30 ) {
                if ( field->getID() == 200000121 ) {
                    ticketID = 4031073;
                    wField = 200000122;
                }
                else if ( field->getID() == 220000110 ) {
                    nItem = inven->itemCount( 4031045 );
                    if ( nItem > 0 ) {
                        ticketID = 4031045;
                        wField = 220000111;
                    } else {
                        ticketID = 4031044;
                        wField = 220000111;
                    }
                }
            }
			nPeople = Field.get(wField)->getUserCount();
			if ( nPeople >= 50 ) self->say( "對不起，目前飛船人數已達上限，為了您的安全考量，請等待下一個班次。");
			else {
				nRet = self->askYesNo( "船已經準備出發了，請加緊腳步。一旦剪票後就無法退票，並且在起飛之前離開飛船都不會補償任何票價。您確定要登船嗎？");
				if ( nRet == 0 ) self->say( "還有事情沒有完成嗎？請加快腳步，船馬上就要離開了。");
				else {
					ret = inventory->exchange( 0, ticketID, -1 );
					if ( ret != 0 ) self->say( "看起來你身上並沒有攜帶正確的票券，請加緊時間購買。" );
					else target->transferField( wField, "" );
				}
			}
		}
	}
	-- // Orbis to Ellinia : 15 min interval or Ellinia to Orbis : 15 min interval
	else if ( field->getID() == 200000111 or field->getID() == 101000300 ) {
		if (  state != 1 ) self->say( "目前飛船尚未抵達，請稍後片刻。飛船抵達時約需要1至5分鐘的準備時間，請耐心等候，謝謝。");
		else if ( boardingTime > cTime ) self->say( "目前飛船停泊中，稍後就會陸續開始檢驗傳票。請耐心等待約1至5分鐘，謝謝您的配合。");
		else {
            wField = field->getID();
            if ( target->getLevel() >= 30 ) {
                if ( field->getID() == 101000300 ) {
                    ticketID = 4031045;
                    wField = 101000301;
                }
                else if ( field->getID() == 200000111 ) {
                    ticketID = 4031047;
                    wField = 200000112;
                }
            }
            else if ( target->getLevel() < 30 ) {
                if ( field->getID() == 101000300 ) {
                    nItem = inven->itemCount( 4031045 );
                    if ( nItem > 0 ) {
                        ticketID = 4031045;
                        wField = 101000301;
                    } else {
                        ticketID = 4031044;
                        wField = 101000301;
                    }
                }
                else if ( field->getID() == 200000111 ) {
                    ticketID = 4031046;
                    wField = 200000112;
                }
            }
			nPeople = Field.get(wField)->getUserCount();
			if ( nPeople >= 50 ) self->say( "對不起，目前飛船人數已達上限，為了您的安全考量，請等待下一個班次。");
			else {
				nRet = self->askYesNo( "船已經準備出發了，請加緊腳步。一旦剪票後就無法退票，並且在起飛之前離開飛船都不會補償任何票價。您確定要登船嗎？");
				if ( nRet == 0 ) self->say( "還有事情沒有完成嗎？請加快腳步，船馬上就要離開了。");
				else {

					ret = inventory->exchange( 0, ticketID, -1 );
					if ( ret != 0 ) self->say( "看起來你身上並沒有攜帶正確的票券，請加緊時間購買。" );
					else target->transferField( wField, "" );
				}
			}
		}
	}
	-- //螃腦綠蝶縑憮 葬?褶僩? 轎嫦?炴? 寡 : 10碟除問 or 葬?褶嘀□? 螃腦綠蝶煎 轎嫦?炴? 寡 : 10碟 除問
	if ( field->getID() ==  200000131 or field->getID() == 240000110 ) {
		if (  state != 1 ) self->say( "目前飛船尚未抵達，請稍後片刻。飛船抵達時約需要1至5分鐘的準備時間，請耐心等候，謝謝。");
		else if ( boardingTime > cTime ) self->say( "目前飛船停泊中，稍後就會陸續開始檢驗傳票。請耐心等待約1至5分鐘，謝謝您的配合。");
		else {
            wField = field->getID();
            if ( target->getLevel() >= 30 ) {
                if ( field->getID() == 200000131 ) {
                    ticketID = 4031331;
                    wField = 200000132;
                }
                else if ( field->getID() == 240000110 ) {
                    ticketID = 4031045;
                    wField = 240000111;
                }
            }
            else if ( target->getLevel() < 30 ) {
                if ( field->getID() == 200000131 ) {
                    nItem = inven->itemCount( 4031331 );
                    if ( nItem > 0 ) {
                        ticketID = 4031331;
                        wField = 200000132;
                    } else {
                        ticketID = 4031330;
                        wField = 200000132;
                    }
                }
                else if ( field->getID() == 240000110 ) {
                    nItem = inven->itemCount( 4031045 );
                    if ( nItem > 0 ) {
                        ticketID = 4031045;
                        wField = 240000111;
                    } else {
                        ticketID = 4031044;
                        wField = 240000111;
                    }
                }
            }
			nPeople = Field.get(wField)->getUserCount();
			if ( nPeople >= 50 ) self->say( "對不起，目前飛船人數已達上限，為了您的安全考量，請等待下一個班次。");
			else {
				nRet = self->askYesNo( "船已經準備出發了，請加緊腳步。一旦剪票後就無法退票，並且在起飛之前離開飛船都不會補償任何票價。您確定要登船嗎？");
				if ( nRet == 0 ) self->say( "還有事情沒有完成嗎？請加快腳步，船馬上就要離開了。");
				else {
					ret = inventory->exchange( 0, ticketID, -1 );
					if ( ret != 0 ) self->say( "看起來你身上並沒有攜帶正確的票券，請加緊時間購買。" );
					else target->transferField( wField, "" );
				}
			}
		}
	}
}

function s_sell_ticket()
{
    field = self->getField();
	nTicket = 0;
	tCost = 0;

	if ( target->getLevel() < 10 ) self->say( "等級必須高於10等以上才能夠購買船票離開。" );
	else {
		nRet = 0;
		--Orbis 
		if ( field->getID() == 200000100 ) {
			v1 = self->askMenu( "嗨，歡迎來到天空之城售票處，這裡有販售到達不同目的地的船票，船馬上就要開了，請問你要購買到哪裡的票呢？\r\n#b#L0# 開往魔法森林#l#k\r\n#b#L1# 開往玩具城#l\r\n#b#L2# 開往神木村#l" );
			if ( v1 == 0 ) {
				if ( target->getLevel() < 30 ) {
					nRet = self->askYesNo( "開往魔法森林的船每15分鐘一個班次，一旦錯過了就必須等待下一個班次。票價一共是#b1.000楓幣#k。你確定要購買#b#t4031046##k嗎？" );
					nTicket = 4031046;
					tCost = 1000;
				}
				else {
					nRet = self->askYesNo( "開往魔法森林的船每15分鐘一個班次，一旦錯過了就必須等待下一個班次。票價一共是#b5.000楓幣#k。你確定要購買#b#t4031047##k嗎？" );
					nTicket = 4031047;
					tCost = 5000;
				}
			}
			else if ( v1 == 1 ) {
				if ( target->getLevel() < 30 ) {
					nRet = self->askYesNo( "開往玩具城的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b2.000楓幣#k。你確定要購買#b#t4031073##k嗎？" );
					nTicket = 4031073;
					tCost = 2000;
				}
				else {
					nRet = self->askYesNo( "開往玩具城的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b6.000楓幣#k。你確定要購買#b#t4031074##k嗎？" );
					nTicket = 4031074;
					tCost = 6000;
				}
			}
			else if ( v1 == 2 ) {
				if ( target->getLevel() < 30 ) {
					nRet = self->askYesNo( "開往神木村的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b10.000楓幣#k。你確定要購買#b#t4031330##k嗎？" );
					nTicket = 4031330;
					tCost = 10000;
				}
				else {
					nRet = self->askYesNo( "開往神木村的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b30.000楓幣#k。你確定要購買#b#t4031331##k嗎？" );
					nTicket = 4031331;
					tCost = 30000;
				}
			}
		}
		--Ludibrium
		else if ( field->getID() == 220000100 ) {
			if ( target->getLevel() < 30 ) {
				nRet = self->askYesNo( "嗨，想要離開了嗎？這裡有販售開往#b天空之城#k的船票。開往#b天空之城#k的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b2.000楓幣#k。你確定要購買#b#t4031044##k嗎？" );
				nTicket = 4031044;
				tCost = 2000;
			}
			else {
				nRet = self->askYesNo( "嗨，想要離開了嗎？這裡有販售開往#b天空之城#k的船票。開往#b天空之城#k的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b6.000楓幣#k。你確定要購買#b#t4031045##k嗎？" );
				nTicket = 4031045;
				tCost = 6000;
			}
		}
		--Ellinia
		else if ( field->getID() == 101000300 ) {
			if ( target->getLevel() < 30 ) {
				nRet = self->askYesNo( "嗨，想要離開了嗎？這裡有販售開往#b天空之城#k的船票。開往#b天空之城#k的飛船每15分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b1.000楓幣#k。你確定要購買#b#t4031044##k嗎？" );
				nTicket = 4031044;
				tCost = 1000;
			}
			else {
				nRet = self->askYesNo( "嗨，想要離開了嗎？這裡有販售開往#b天空之城#k的船票。開往#b天空之城#k的飛船每15分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b5.000楓幣#k。你確定要購買#b#t4031045##k嗎？" );
				nTicket = 4031045;
				tCost = 5000;
			}
		}
		else if ( field->getID() == 240000100 ) {
			if ( target->getLevel() < 30 ) {
				nRet = self->askYesNo( "嗨，想要離開了嗎？這裡有販售開往#b天空之城#k的船票。開往#b天空之城#k的飛船每10分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b2.000楓幣#k。你確定要購買#b#t4031044##k嗎？" );
				nTicket = 4031044;
				tCost = 2000;
			}
			else {
				nRet = self->askYesNo( "嗨，想要離開了嗎？這裡有販售開往#b天空之城#k的船票。開往#b天空之城#k的飛船每15分鐘一個班次，錯過了就必須等待下一個班次。票價一共是#b30.000楓幣#k。你確定要購買#b#t4031045##k嗎？" );
				nTicket = 4031045;
				tCost = 30000;
			}
		}

		if ( nRet == 0 ) self->say( "你還有事情沒完成嗎？等你下定決心以後再來找我吧。" );
		else {
			ret = inventory->exchange( -tCost, nTicket, 1 );
			if ( ret !=0 ) self->say( "你確定身上有攜帶#b" .. tCost .. "楓幣#k？或者你的背包欄位已經滿了。" );
		}
	}
}

-- The NPC that warps the character to the different boarding platform
function s_getAboard() {
	v = self->askMenu( "這裡通往數個不同的月台，每個都將通往不同的目的地，時間有限，請問你要到哪一個月台呢？\r\n#b#L0#開往魔法森林的月台#l\r\n#b#L1#開往玩具城的月台#l#k\r\n#b#L2#開往神木村的月台#l#k" );
	if ( v == 0 ) {
		nRet = self->askYesNo( "在進入月台之前請確保您已攜帶正確的船票。您確定要前往#b通往魔法森林的月台#k？" );
		if ( nRet == 0 ) self->say( "看來你還有事情要做對吧？請加快腳步，以免錯過班次！" );
		else if ( nRet == 1 ) target->transferField( 200000110, "west00" );
	}
	else if ( v == 1 ) {
		nRet = self->askYesNo( "在進入月台之前請確保您已攜帶正確的船票。您確定要前往通往#b通往玩具城的月台#k？" );
		if ( nRet == 0 ) self->say( "看來你還有事情要做對吧？請加快腳步，以免錯過班次！" );
		else if ( nRet == 1 ) target->transferField( 200000120, "west00" );
	}	
	else if ( v == 2 ) {
		nRet = self->askYesNo( "在進入月台之前請確保您已攜帶正確的船票。您確定要前往通往#b通往神木村的月台#k？" );
		if ( nRet == 0 ) self->say( "看來你還有事情要做對吧？請加快腳步，以免錯過班次！" );
		else if ( nRet == 1 ) target->transferField( 200000130, "west00" );
	}	
	else if ( v == 3 ) {
		nRet = self->askYesNo( "在進入月台之前請確保您已攜帶正確的船票。您確定要前往通往#b通往武陵的月台#k？" );
		if ( nRet == 0 ) self->say( "看來你還有事情要做對吧？請加快腳步，以免錯過班次！" );
		else if ( nRet == 1 ) target->transferField( 200000140, "west00" );
	}
}
function s_goOutWaitingRoom() {
	field = self->getField();

	nRet = self->askYesNo( "你確定要離開這裡嗎？一旦離開不但沒有任何補償，還必須重新購買船票才能回來，你確定嗎？");
	if ( nRet == 0 ) self->say( "馬上就要出發了，請耐心等候。" );
	else {
		if ( field->getID() == 200000122 ) target->transferField( 200000100, "" );
		else if ( field->getID() == 220000111 ) target->transferField( 220000100, "" );
		else if ( field->getID() == 101000301 ) target->transferField( 101000300, "" );
		else if ( field->getID() == 200000112 ) target->transferField( 200000100, "" );
		else if ( field->getID() == 240000111 ) target->transferField( 240000100, "" );
		else if ( field->getID() == 200000132 ) target->transferField( 200000100, "" );		
	}
}