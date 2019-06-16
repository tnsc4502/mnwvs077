require "./DataSrv/Script/sysDef"


function party2_takwawayitem() {
	inven = inventory;

	count1 = inven->itemCount( 4001022 );
	if ( count1 > 0 ) inven->exchange( 0, 4001022, -count1 );
	count2 = inven->itemCount( 4001023 );
	if ( count2 > 0 ) inven->exchange( 0, 4001023, -count2 );

	return;
}

--// ?民鶳? : 2040034
function s_party2_enter() {
	if ( target->isPartyBoss() != 1 ) {
		self->say( "想要進入充滿危險以及各項挑戰的地圖挑戰組隊任務嗎？但是要挑戰玩具城101組隊任務，單靠自己一個人是不可能的，所以請先加入隊伍後再與我談話。" );
		return;
	}
	setParty = FieldSet.get("Party2");
	if(setParty) 
	{
		inventory->exchange(0, 4001022, -10)
		ret = setParty->enter(userID); 
		if (ret == 1)
			self->say("想要進入充滿危險以及各項挑戰的地圖挑戰組隊任務嗎？但是要挑戰玩具城101組隊任務，單靠自己一個人是不可能的，所以請先加入隊伍後再與我談話。");
		else if (ret == 2)
			self->say("請隊長與我談話。");
		else if (ret == 3 or ret == 4)
			self->say("欲進行玩具城101組隊任務，您的隊伍人數至少#r5人#k，等級界於#r36至50級#k之間，並且由隊長與我交談！");
		else if (ret == 5)
			self->say("已經有隊伍目前正在挑戰組隊任務，請更換頻道或等待進行的隊伍結束。");
		else
			party2_takwawayitem();
	}
}

function party2_help() {
	field = self->getField();
	if ( field->getID() == 922010100 ) self->say( "歡迎來到第1關卡。在這個地圖中四處充滿著怪物，打倒這些怪物可以獲得#b#t4001022##k，雖然這些怪物看起來十分常見，但卻擁有更強大的力量，因此千萬不能輕敵。\r\n請你們同心協力收集#b25張#t4001022##k，並且統一由隊長交給我，一旦數量正確，就可以前往下一個關卡，祝你們好運！" );
	else if ( field->getID() == 922010200 ) self->say( "歡迎來到第2關卡。在這個地圖中你可以看到好幾個箱子，擊破這些箱子可能會獲得#b#t4001022##k或者被傳送到隱藏地圖中。請到各處蒐集#b15張#t4001022##k，並且由隊長統一交給我。即使被傳送到了隱藏地圖，仍然可以找到會掉落#t4001022#的箱子，如果你離開了隱藏地圖將無法再度回去，所以如果沒有把所有的#t4001022#帶出來，那只能重新開始任務！" );
	else if ( field->getID() == 922010300 ) self->say( "歡迎來到第3關卡。在這個地圖中充滿著怪物與箱子，如果你擊敗了這些怪物，它們會掉落#b#t4001022##k，而如果你擊破了箱子，則會召喚會掉落相同物品的怪物。請你們蒐集所有的#b#t4001022##k。正確的數量我已經告知給隊長，隊員們可以幫忙一起蒐集後統一交給隊長，並且轉交給我，祝你們好運！" );
	else if ( field->getID() == 922010400 ) self->say( "歡迎來到第4關卡。在下面你可以發現有數個通往黑暗空間的入口，在那個空間裡面有著名為#b#o9300008##k的怪物。這些怪物藏身於黑暗之中，難以察覺，如果你沒有仔細尋找，很容易就會忽視它們。請你們擊敗所有的怪物，並且蒐集#b6張#t4001022##k給我，如果發現蒐集的數量不對，一定是沒有仔細的尋找#b#o9300008##k，它們藏身於黑暗之中，請更加耐心的觀察。祝你們好運！" );
	else if ( field->getID() == 922010500 ) self->say( "歡迎來到第5關卡。在這裡你可以看到好幾個通往隱藏地圖的入口，請你們前往不同的隱藏地圖蒐集#b24張#t4001022##k。請注意：有些入口只有特定職業才有辦法進入，例如其中一個入口外圍被怪物#b#o9300013##k阻擋著，只有盜賊才有可能通過，而另一個入口也只有法師職業能夠抵達，總之，祝你們好運！" );
	else if ( field->getID() == 922010600 ) self->say( "歡迎來到第6關卡。在這裡你可以看到印著數字的箱子，其中一些箱子可以通往更高的台階，而相對的，有些則可能把你送回起始點。。請找到這確的箱子組合，並且抵達地圖最頂端，祝你們好運了！" );
	else if ( field->getID() == 922010700 ) self->say( "歡迎來到第7關卡。在這個關卡中有著異常強大的#b#o9300010##k，請你們同心協力擊敗它們，並且蒐集所有的#b#t4001022##k給我。要召喚出#o9300010#，必須先用遠距離攻擊擊倒箱子附近的怪物，才會召喚出它們。這些怪物非常強大，請務必小心謹慎以免受重傷！祝你們好運！" );
	else if ( field->getID() == 922010800 ) self->say( "歡迎來到第8關卡。在平台的最頂端有通往下一個關卡的箱子，請你們在這些箱子中找出5個箱子來組成正確的答案，並且站在這些箱子上面，接著由隊長來跟我確認答案是否正確。請注意，#r離箱子太遠或者站的太邊緣#k可能不會被算入答案中，請小心！" );
	else if ( field->getID() == 922010900 ) self->say( "歡迎來到第9關卡。這裡是最後的挑戰了，請你們把右邊的怪物擊倒，接著會召喚出可怕的#b#o9300012##k。請你們同心協力把它擊敗，並且取回#b#t4001023##k給我，這樣就能完成整個任務，而我就可以送你們離開，請你們小心，這個怪物十分強大！祝你們好運！" );
	else if ( field->getID() == 922011000 ) self->say( "恭喜你們來到了獎勵關卡！真不敢相信你們真的打敗了#b#o9300012##k！在這個地圖中你們可以看到到處充滿著箱子，箱子裡面裝著驚喜等著你們！時間不多了，趕緊擊破箱子蒐集寶物吧！如果你運氣夠好，說不定還能蒐集到價值不斐的武器呢！" );
}

function check_stage( st, checkall ) {
	quest = FieldSet.get("Party2");
	stage = quest->getVar( "stage" );
	if ( stage != st ) {
		self->say( "關卡已經完成，通往下一個關卡的傳送門已經開啟，請加緊腳步前往。" );
		return 0;
	}

	field = self->getField();
	if ( checkall == 1 and quest->getUserCount() != field->getUserCount() ) {
		self->say( "所有隊伍成員必須都在場。" );
		return 0;
	}
	return 1;
}

function area_check( num, user ) {
	field = self->getField();
	count = 0;
	answer = "";
	for i = 1, num  {
		cur = field->countUserInArea( string( i-1 ) );
		count = count + cur;
		answer = answer + string( cur );
	}
	if ( count == user ) return answer;
	return "";
}

--// 溯萄 弊琿 : 2040036
function party2_stage1() {
	quest = FieldSet.get("Party2");
	stage = quest->getVar( "stage" );
	if ( stage == "" ) {
		quest->setVar( "stage", "1" );
		self->say( "歡迎來到第一關卡，在這裡你可以看到四處充滿著怪物，請你擊敗所有的怪物並且收集#b25張#t4001022##k，並且交給我。隊伍成員可以分工合作蒐集#t4001022#，並且統一由隊長交給我。這些怪物雖然看起來非常熟悉，但可能比你預期的還要更加強大，請務必小心！" );
		return;
	}
	if ( stage != "1" ) {
		self->say( "恭喜你完成了第一階段的關卡，傳送門已經開啟，請趕緊進入下一個關卡。" );
		return;
	}	

	inven = inventory;
	if ( inven->itemCount( 4001022 ) < 25 ) self->say( "歡迎來到第一關卡，在這裡你可以看到四處充滿著怪物，請你擊敗所有的怪物並且收集#b25張#t4001022##k，並且交給我。隊伍成員可以分工合作蒐集#t4001022#，並且統一由隊長交給我。這些怪物雖然看起來非常熟悉，但可能比你預期的還要更加強大，請務必小心！" );
	else {
		if ( inven->exchange( 0, 4001022, -25 ) != 0 ) {
			self->say( "你身上似乎沒有#b25張#t4001022##k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "2" );
		quest->incExpAll( 3000, 7010 );
		self->say( "通往下一個關卡的傳送門已經開啟，請趕緊往下一個關卡移動。" );
		return;
	}
}

--// 螃溶雖 弊琿 : 2040037
function party2_stage2() {
	if ( check_stage( "2", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");	

	inven = inventory;
	if ( inven->itemCount( 4001022 ) < 15 ) self->say( "嗨！歡迎來到第二關卡。在下面你可以看到數個箱子，擊破這些箱子可能被傳送到隱藏地圖或者獲得#t4001022#。請找出#b15張#t4001022##k並且交給我。隊伍成員們可以共同蒐集#t4001022#，並且統一由隊長交給我即可，祝你們好運！" );
	else {
		if ( inven->exchange( 0, 4001022, -15 ) != 0 ) {
			self->say( "你身上似乎沒有#b15張#t4001022#s#k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "3" );
		quest->incExpAll( 3600, 7010 );
		self->say( "通往下一個關卡的傳送門已經開啟，請趕緊往下一個關卡移動。" );
		return;
	}
}

--// 蕃煎辦 弊琿 : 2040038
function party2_stage3() {
	if ( check_stage( "3", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");	

	inven = inventory;
	if ( inven->itemCount( 4001022 ) != 32 ) self->say( "嗨！歡迎來到第三關卡。在這裡你可以看到四處充滿著怪物與箱子。這些怪物會掉落#b#t4001022##k，就如同其他關卡。如果箱子被擊破，也會召喚出相同的怪物。這個關卡需要蒐集的#b#t4001022##k數量是以下題目的答案：\r\n請問#r初心者初始血量 減去 法師一轉最低等級 再減去 盜賊一轉最低等級#k是多少？請蒐集與答案相同數量個#b#t4001022##k給我。隊伍成員們可以分工合作一起蒐集，最後再由隊長交給我即可！祝你們好運。" );
	else {
		if ( inven->exchange( 0, 4001022, -32 ) != 0 ) {
			self->say( "你身上似乎沒有#b32張#t4001022##k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "4" );
		quest->incExpAll( 4200, 7010 );
		self->say( "通往下一個關卡的傳送門已經開啟，請趕緊往下一個關卡移動。" );
		return;
	}
}

--// 蕃煎辦斜萼 弊琿 : 2040039
function party2_stage4() {
	if ( check_stage( "4", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");

	inven = inventory;
	if ( inven->itemCount( 4001022 ) < 6 ) self->say( "嗨，歡迎來到一四關卡。在下面有數個通往黑暗空間的入口，在裡面隱藏著#b#o9300008##k。這些怪物會短暫出現並且馬上消失，所以很難察覺。請擊敗這些怪物且收集#b6張#t4001022##k。\r\n請記住，就像我說的，這些怪物很難察覺，因此必須仔細並且耐心地在黑暗中觀察，隊伍成員們可以合作收集#b#t4001022##k，並且統一給予隊長後再交給我。祝你們好運了！" );
	else {
		if ( inven->exchange( 0, 4001022, -6 ) != 0 ) {
			self->say( "你身上似乎沒有#b6張#t4001022##k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "5" );
		quest->incExpAll( 4800, 7010 );
		self->say( "通往下一個關卡的傳送門已經開啟，請趕緊往下一個關卡移動。" );
		return;
	}
}

--// 斜萼 弊琿 : 2040040
function party2_stage5() {
	if ( check_stage( "5", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");

	inven = inventory;
	if ( inven->itemCount( 4001022 ) < 24 ) self->say( "嗨，歡迎來到第五關卡！在這個地圖中有幾個通往其他隱藏地圖的入口，請你們到各個地圖中蒐集#b24張#t4001022##k並且帶回來給我。請注意，有些入口只有特定職業才能夠進入蒐集#b#t4001022##k。例如其中一個入口之外，有名為#b#o9300013##k的怪物阻擋著，只有盜賊能夠閃躲它們。而另一條路，只有法師職業才能夠到達，總之，祝你們好運！" );
	else {
		if ( inven->exchange( 0, 4001022, -24 ) != 0 ) {
			self->say( "你身上似乎沒有#b24 #t4001022#s#k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "7" );
		quest->incExpAll( 5400, 7010 );
		self->say( "通往下一個關卡的傳送門已經開啟，請趕緊往下一個關卡移動。" );
		return;
	}
}

--// 嬴瓔嬴 弊琿 : 2040041
function party2_stage6() {
	--qr = target.questRecord;
	val = qr->get( 7011 );
	quest = FieldSet.get("Party2");
	field = self->getField();

	if ( quest->getUserCount() != field->getUserCount() ) {
		self->say( "請確保所有隊員都在場。" );
		return;
	}

	if ( val == "" ) {
		qr->setState( 7011, 1, "1" );
		self->say( "嗨！歡迎來到第六關卡。在這裡你可以看到印著數字的箱子，其中一些箱子可以通往更高的台階，而相對的，有些則可能把你送回起始點。請找到這確的箱子組合，並且抵達地圖最頂端，祝你們好運了！\r\n\r\n什麼？需要我提示嗎？好吧，那麼，請記住：#r1–3–3–2–2–1–3–3–3–1–2–3–1–1–1#k" );	
	}
	else if ( val == "1" ) {
		qr->setState( 7011, 1, "e" );
		self->say( "嗨！歡迎來到第六關卡。在這裡你可以看到印著數字的箱子，其中一些箱子可以通往更高的台階，而相對的，有些則可能把你送回起始點。請找到這確的箱子組合，並且抵達地圖最頂端，祝你們好運了！#k\r\n\r\n什麼？需要我提示嗎？好吧，那麼，請牢記：#r1–3–3–2–2–1–3–3–3–1–2–3–1–1–1#k，這是最後一次機會了！" );	
	}
	else self->say( "嗨！歡迎來到第六關卡。在這裡你可以看到印著數字的箱子，其中一些箱子可以通往更高的台階，而相對的，有些則可能把你送回起始點。。請找到這確的箱子組合，並且抵達地圖最頂端，祝你們好運了！#k\r\n\r\n對不起，最多只能提示兩次！" );	
}

--// 蝶蘋檜綰瑞 弊琿 : 2040042
function party2_stage7() {
	if ( check_stage( "7", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");

	inven = inventory;
	if ( inven->itemCount( 4001022 ) < 3 ) self->say( "歡迎來到第七關卡！在這個關卡中充滿著#b#o9300010##k這些強大的怪物，請你們擊敗這些怪物並且蒐集所有的#b#t4001022##k。\r\n沒有看到#o9300010#嗎？必須使用遠距離攻擊擊敗上面的怪物，並且觸發箱子後就會召喚出這些強大的怪物。請務必小心謹慎，這些怪物真的非常強大，一不小心就會受傷的！祝你們好運了！" );
	else {
		if ( inven->exchange( 0, 4001022, -3 ) != 0 ) {
			self->say( "你身上似乎沒有#b3 #t4001022#s#k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "8" );
		quest->incExpAll( 6600, 7010 );
		self->say( "通往下一個關卡的傳送門已經開啟，請趕緊往下一個關卡移動。" );
		return;
	}
}

function make_area_answer(numCount, answerCount) {
    rndSeq = randomSeq(1, numCount);
    ret = "";
    for k, v in pairs(rndSeq) {
        if(v > answerCount)
            ret = ret .. '0';
        else
            ret = ret .. '1';
    }
    return ret;
}

function area_check(areaCount, answerCount) {
    ret = "";
    areaCount = areaCount - 1;
    count = 0;
    for i = 0, areaCount {
        inArea = field->countUserInArea(i);
        count = count + inArea;
        ret = ret .. inArea;
    }
    if(count != answerCount) 
        return "";
    return ret;
}

--// 綰瑞 弊琿 : 2040043
function party2_stage8() {
	if ( check_stage( "8", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");
	question = quest->getVar( "ans" );
	if ( question == "" ) {
		quest->setVar( "ans", make_area_answer( 9, 5 ) );
		self->say( "歡迎來到第八關卡。在這邊你可以看到數個箱子，其中#b5#k個箱子能夠開啟通往下一個關卡的傳送點，請確保#b5#k位隊伍成員都站在不同的箱子上，並且由隊長來與我確認答案。請注意，站的太靠近邊緣可能不會被計算到答案中，祝你們好運！" );
		return;
	}

	field = self->getField();
	answer = area_check( 9, 5 );

	if ( answer == "" ) self->say( "我想你們並沒有找出5個正確的箱子組合，請再多加嘗試。請記住，必須要有5位成員分別站在不同的箱子上，如果站的離中心點太遠，可能不會被計算入答案中，祝你好運！" );
	else if ( question != answer ) {
		field->effectScreen( "quest/party/wrong_kor" );
		field->effectSound( "Party1/Failed" );
	}
	else {
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		field->enablePortal( "next00", 1 );
		quest->setVar( "stage", "9" );
		quest->incExpAll( 7200, 7010 );
	}
}

--// 夥檜螢滇 弊琿 : 2040043
function party2_stage9() {
	if ( check_stage( "9", 1 ) == 0 ) return;

	quest = FieldSet.get("Party2");

	inven = inventory;
	if ( inven->itemCount( 4001023 ) < 1 ) self->say( "沒想到你們竟然能夠抵達這裡！現在，這是最後的挑戰了。看看右邊的平台上，有一隻怪物，請將他擊倒，接著會召喚出可怕的#b#o9300012##k。這個怪物非常的強大可怕，必須要團隊成員同心協力才有可能打敗它！\r\n你們的任務就是擊敗它，並且取回#b#t4001023##k並帶來給我，我就能夠將你們送離這裡，我相信你們辦的到的，祝你們好運！" );
	else {
		if ( inven->exchange( 0, 4001023, -1 ) != 0 ) {
			self->say( "你身上似乎沒有#b#t4001022##k。請確認數量正確後再與我交談！" );
			return;
		}
		field = self->getField();
		field->effectScreen( "quest/party/clear" );
		field->effectSound( "Party1/Clear" );
		field->effectObject( "gate" );
		quest->setVar( "stage", "clear" );		
		quest->incExpAll( 8500, 7010 );
		quest->transferAll(922011000, "");
		--setParty = FieldSet( "Party4" );
		--res = setParty.enter( target.nCharacterID, 0 );
		--if ( res == -1 or res == 4 ) self->say( "Tenho motivos para n緌 deixar voc?entrar. Por favor, tente mais tarde." );
		--if ( res == 1 ) self->say( "Acho que voc?n緌 est?em um grupo. Voc?precisa estar em um grupo para participar desta miss緌." );
		return;
	}
}

--// 爾鼻
function party2_reward() {
	self->say( "恭喜你們完成了所有關卡！我真的非常佩服你們！在你們享受勝利的同時，我也想送你們一些小禮物作為回報，在接受之前，請確認你的背包內還有足夠的欄位。" );

	inven = inventory;
	if ( inven->slotCount( 2 ) > inven->holdCount( 2 ) and inven->slotCount( 4 ) > inven->holdCount( 4 ) ) {
		rnum = random( 0, 250 );
		nNewItemID = 0;
		nNewItemNum = 0;
		if ( rnum == 0 ) { nNewItemID = 2000004; nNewItemNum = 10; }
		else if ( rnum == 1 ) { nNewItemID = 2000002; nNewItemNum = 100; }
		else if ( rnum == 2 ) { nNewItemID = 2000003; nNewItemNum = 100; }
		else if ( rnum == 3 ) { nNewItemID = 2000006; nNewItemNum = 30; }
		else if ( rnum == 4 ) { nNewItemID = 2022000; nNewItemNum = 30; }
		else if ( rnum == 5 ) { nNewItemID = 2022003; nNewItemNum = 30; }
		else if ( rnum == 6 ) { nNewItemID = 2040002; nNewItemNum = 1; }
		else if ( rnum == 7 ) { nNewItemID = 2040402; nNewItemNum = 1; }
		else if ( rnum == 8 ) { nNewItemID = 2040502; nNewItemNum = 1; }
		else if ( rnum == 9 ) { nNewItemID = 2040505; nNewItemNum = 1; }
		else if ( rnum == 10 ) { nNewItemID = 2040602; nNewItemNum = 1; }
		else if ( rnum == 11 ) { nNewItemID = 2040802; nNewItemNum = 1; }
		else if ( rnum == 12 ) { nNewItemID = 4003000; nNewItemNum = 50; }
		else if ( rnum == 13 ) { nNewItemID = 4010000; nNewItemNum = 15; }
		else if ( rnum == 14 ) { nNewItemID = 4010001; nNewItemNum = 15; }
		else if ( rnum == 15 ) { nNewItemID = 4010002; nNewItemNum = 15; }
		else if ( rnum == 16 ) { nNewItemID = 4010003; nNewItemNum = 15; }
		else if ( rnum == 17 ) { nNewItemID = 4010004; nNewItemNum = 15; }
		else if ( rnum == 18 ) { nNewItemID = 4010005; nNewItemNum = 15; }
		else if ( rnum == 19 ) { nNewItemID = 4010006; nNewItemNum = 10; }
		else if ( rnum == 20 ) { nNewItemID = 4020000; nNewItemNum = 15; }
		else if ( rnum == 21 ) { nNewItemID = 4020001; nNewItemNum = 15; }
		else if ( rnum == 22 ) { nNewItemID = 4020002; nNewItemNum = 15; }
		else if ( rnum == 23 ) { nNewItemID = 4020003; nNewItemNum = 15; }
		else if ( rnum == 24 ) { nNewItemID = 4020004; nNewItemNum = 15; }
		else if ( rnum == 25 ) { nNewItemID = 4020005; nNewItemNum = 15; }
		else if ( rnum == 26 ) { nNewItemID = 4020006; nNewItemNum = 15; }
		else if ( rnum == 27 ) { nNewItemID = 4020007; nNewItemNum = 6; }
		else if ( rnum == 28 ) { nNewItemID = 4020008; nNewItemNum = 6; }
		else if ( rnum == 29 ) { nNewItemID = 1032002; nNewItemNum = 1; }
		else if ( rnum == 30 ) { nNewItemID = 1032011; nNewItemNum = 1; }
		else if ( rnum == 31 ) { nNewItemID = 1032008; nNewItemNum = 1; }
		else if ( rnum == 32 ) { nNewItemID = 1102011; nNewItemNum = 1; }
		else if ( rnum == 33 ) { nNewItemID = 1102012; nNewItemNum = 1; }
		else if ( rnum == 34 ) { nNewItemID = 1102013; nNewItemNum = 1; }
		else if ( rnum == 35 ) { nNewItemID = 1102014; nNewItemNum = 1; }
		else if ( rnum == 36 ) { nNewItemID = 2040803; nNewItemNum = 1; }
		else if ( rnum == 37 ) { nNewItemID = 2070011; nNewItemNum = 1; }
		else if ( rnum == 38 ) { nNewItemID = 2043001; nNewItemNum = 1; }
		else if ( rnum == 39 ) { nNewItemID = 2043101; nNewItemNum = 1; }
		else if ( rnum == 40 ) { nNewItemID = 2043201; nNewItemNum = 1; }
		else if ( rnum == 41 ) { nNewItemID = 2043301; nNewItemNum = 1; }
		else if ( rnum == 42 ) { nNewItemID = 2043701; nNewItemNum = 1; }
		else if ( rnum == 43 ) { nNewItemID = 2043801; nNewItemNum = 1; }
		else if ( rnum == 44 ) { nNewItemID = 2044001; nNewItemNum = 1; }
		else if ( rnum == 45 ) { nNewItemID = 2044101; nNewItemNum = 1; }
		else if ( rnum == 46 ) { nNewItemID = 2044201; nNewItemNum = 1; }
		else if ( rnum == 47 ) { nNewItemID = 2044301; nNewItemNum = 1; }
		else if ( rnum == 48 ) { nNewItemID = 2044401; nNewItemNum = 1; }
		else if ( rnum == 49 ) { nNewItemID = 2044501; nNewItemNum = 1; }
		else if ( rnum == 50 ) { nNewItemID = 2044601; nNewItemNum = 1; }
		else if ( rnum == 51 ) { nNewItemID = 2044701; nNewItemNum = 1; }
		else if ( rnum == 52 ) { nNewItemID = 2000004; nNewItemNum = 15; }
		else if ( rnum == 53 ) { nNewItemID = 2000002; nNewItemNum = 80; }
		else if ( rnum == 54 ) { nNewItemID = 2000003; nNewItemNum = 80; }
		else if ( rnum == 55 ) { nNewItemID = 2000006; nNewItemNum = 25; }
		else if ( rnum == 56 ) { nNewItemID = 2022000; nNewItemNum = 25; }
		else if ( rnum == 57 ) { nNewItemID = 2022003; nNewItemNum = 25; }
		else if ( rnum == 58 ) { nNewItemID = 4003000; nNewItemNum = 55; }
		else if ( rnum == 59 ) { nNewItemID = 4010000; nNewItemNum = 12; }
		else if ( rnum == 60 ) { nNewItemID = 4010001; nNewItemNum = 12; }
		else if ( rnum == 61 ) { nNewItemID = 4010002; nNewItemNum = 12; }
		else if ( rnum == 62 ) { nNewItemID = 4010003; nNewItemNum = 12; }
		else if ( rnum == 63 ) { nNewItemID = 4010004; nNewItemNum = 12; }
		else if ( rnum == 64 ) { nNewItemID = 4010005; nNewItemNum = 12; }
		else if ( rnum == 65 ) { nNewItemID = 4010006; nNewItemNum = 8; }
		else if ( rnum == 66 ) { nNewItemID = 4020000; nNewItemNum = 12; }
		else if ( rnum == 67 ) { nNewItemID = 4020001; nNewItemNum = 12; }
		else if ( rnum == 68 ) { nNewItemID = 4020002; nNewItemNum = 12; }
		else if ( rnum == 69 ) { nNewItemID = 4020003; nNewItemNum = 12; }
		else if ( rnum == 70 ) { nNewItemID = 4020004; nNewItemNum = 12; }
		else if ( rnum == 71 ) { nNewItemID = 4020005; nNewItemNum = 12; }
		else if ( rnum == 72 ) { nNewItemID = 4020006; nNewItemNum = 12; }
		else if ( rnum == 73 ) { nNewItemID = 4020007; nNewItemNum = 4; }
		else if ( rnum == 74 ) { nNewItemID = 4020008; nNewItemNum = 4; }
		else if ( rnum == 75 ) { nNewItemID = 2040001; nNewItemNum = 1; }
		else if ( rnum == 76 ) { nNewItemID = 2040004; nNewItemNum = 1; }
		else if ( rnum == 77 ) { nNewItemID = 2040301; nNewItemNum = 1; }
		else if ( rnum == 78 ) { nNewItemID = 2040401; nNewItemNum = 1; }
		else if ( rnum == 79 ) { nNewItemID = 2040501; nNewItemNum = 1; }
		else if ( rnum == 80 ) { nNewItemID = 2040504; nNewItemNum = 1; }
		else if ( rnum == 81 ) { nNewItemID = 2040601; nNewItemNum = 1; }
		else if ( rnum == 82 ) { nNewItemID = 2040601; nNewItemNum = 1; }
		else if ( rnum == 83 ) { nNewItemID = 2040701; nNewItemNum = 1; }
		else if ( rnum == 84 ) { nNewItemID = 2040704; nNewItemNum = 1; }
		else if ( rnum == 85 ) { nNewItemID = 2040707; nNewItemNum = 1; }
		else if ( rnum == 86 ) { nNewItemID = 2040801; nNewItemNum = 1; }
		else if ( rnum == 87 ) { nNewItemID = 2040901; nNewItemNum = 1; }
		else if ( rnum == 88 ) { nNewItemID = 2041001; nNewItemNum = 1; }
		else if ( rnum == 89 ) { nNewItemID = 2041004; nNewItemNum = 1; }
		else if ( rnum == 90 ) { nNewItemID = 2041007; nNewItemNum = 1; }
		else if ( rnum == 91 ) { nNewItemID = 2041010; nNewItemNum = 1; }
		else if ( rnum == 92 ) { nNewItemID = 2041013; nNewItemNum = 1; }
		else if ( rnum == 93 ) { nNewItemID = 2041016; nNewItemNum = 1; }
		else if ( rnum == 94 ) { nNewItemID = 2041019; nNewItemNum = 1; }
		else if ( rnum == 95 ) { nNewItemID = 2041022; nNewItemNum = 1; }
		else if ( rnum >= 96 and rnum <= 150 ) { nNewItemID = 2000004; nNewItemNum = 10; }
		else if ( rnum >= 151 and rnum <= 200 ) { nNewItemID = 2000002; nNewItemNum = 100; }
		else { nNewItemID = 2000003; nNewItemNum = 100; }

		count1 = inven->itemCount( 4001022 );
		if ( count1 > 0 ) {
			if ( inven->exchange( 0, 4001022, -count1 ) != 0 ) {
				self->say( "你身上還有#t4001022#嗎？請將它們交給我。" );
				return;
			}
		}
		count2 = inven->itemCount( 4001023 );
		if ( count2 > 0 ) {
			if ( inven->exchange( 0, 4001023, -count2 ) != 0 ) {
				self->say( "你身上還有#t4001023#嗎？請將它們交給我。" );
				return;
			}
		}

		ret = inven->exchange( 0, nNewItemID, nNewItemNum );
		if ( ret != 0 ) self->say( "嗯...你確定你的背包還有剩餘的空間嗎？如果背包已滿我就無法給你任何獎勵了..." );
		else {
			--qr = target.questRecord;
			qr->setState( 7010, 1, "1" );
			qr->setState( 7011, 0 );
			target->transferField( 221024500, "" );
		}
	}
	else self->say( "請確保你的消耗與其他欄還有足夠的空間，否則我將無法給你獎勵。" );
}

--// 弊琿 - 啪歜 霞?? NPC
function s_party2_play() {
	field = self->getField();
	quest = FieldSet.get("Party2");

	--// 爾鼻
	if ( field->getID() == 922011100 ) {
		party2_reward();
		return;
	}

	if ( target->isPartyBoss() != 1 ) party2_help();
	else {
		if ( field->getID() == 922010100 ) party2_stage1();
		else if ( field->getID() == 922010200 ) party2_stage2();
		else if ( field->getID() == 922010300 ) party2_stage3();
		else if ( field->getID() == 922010400 ) party2_stage4();
		else if ( field->getID() == 922010500 ) party2_stage5();
		else if ( field->getID() == 922010600 ) party2_stage6();
		else if ( field->getID() == 922010700 ) party2_stage7();
		else if ( field->getID() == 922010800 ) party2_stage8();
		else if ( field->getID() == 922010900 ) party2_stage9();
		else if ( field->getID() == 922011000 ) party2_help();
	}
}

--// 煽薑 撻渦蝦 - 黴濰衛麵輿朝 NPC
function s_party2_out() {
	--qr = target.questRecord;
	field = self->getField();

	if ( field->getID() == 922010000 ) {
		inven = inventory;
		count1 = inven->itemCount( 4001022 );
		if ( count1 > 0 ) {
			if ( inven->exchange( 0, 4001022, -count1 ) != 0 ) {
				self->say( "你身上還有#t4001022#嗎？請將它們交給我。" );
				return;
			}
		}
		count2 = inven->itemCount( 4001023 );
		if ( count2 > 0 ) {
			if ( inven->exchange( 0, 4001023, -count2 ) != 0 ) {
				self->say( "你身上還有#t4001023#嗎？請將它們交給我。" );
				return;
			}
		}
		qr->setState( 7011, 0 );
		target->transferField( 221024500, "" );
	}
	else {
		--// 陝 蝶纔檜雖縑憮 黴濰裘戲煎 爾魚棻
		nRet = self->askYesNo( "你確定要離開這裡嗎？一旦離開後就必須重新來過，你確定嗎？");
		if ( nRet == 0 ) self->say( "我明白了，請你繼續加油。" );
		else {
			qr->setState( 7011, 0 );
			target->transferField( 922010000, "" );
		}
	}
}
