require "./DataSrv/Script/sysDef"

function chat_message4(  index,  makeItem,  needItem,  reqLevel ) {
	
	nRet = self->askYesNo( "想要製作 " .. makeItem .. " 你必須有足夠的材料。 O limite de n癉el ?" .. reqLevel .. "e por favor, certifique-se de que n緌 vai usar um item que est?sendo aperfei蔞ado como material. O que voc?acha? Voc?quer um?\r\n\r\n#b" .. needItem );
	if ( nRet == 0 ) self->say( "身上沒有足夠的材料或者楓幣嗎？那我勸你可以到各個城鎮尋找必須的材料，等到蒐集完成後再回來找我。" );
	else {
		if ( index == 1 ) ret = inventory->exchange( -20000, 1082007, -1, 4011001, -1, 1082005, 1 );
		else if ( index == 2 ) ret = inventory->exchange( -25000, 1082007, -1, 4011005, -2, 1082006, 1 );
		else if ( index == 3 ) ret = inventory->exchange( -30000, 1082008, -1, 4021006, -3, 1082035, 1 );
		else if ( index == 4 ) ret = inventory->exchange( -40000, 1082008, -1, 4021008, -1, 1082036, 1 );
		else if ( index == 5 ) ret = inventory->exchange( -45000, 1082023, -1, 4011003, -4, 1082024, 1 );
		else if ( index == 6 ) ret = inventory->exchange( -50000, 1082023, -1, 4021008, -2, 1082025, 1 );
		else if ( index == 7 ) ret = inventory->exchange( -55000, 1082009, -1, 4011002, -5, 1082010, 1 );
		else if ( index == 8 ) ret = inventory->exchange( -60000, 1082009, -1, 4011006, -4, 1082011, 1 );
		else if ( index == 9 ) ret = inventory->exchange( -70000, 1082059, -1, 4011002, -3, 4021005, -5, 1082060, 1 );
		else if ( index == 10 ) ret = inventory->exchange( -80000, 1082059, -1, 4021007, -2, 4021008, -2, 1082061, 1 );

		if ( ret != 0 ) self->say( "Por favor, verifique com cuidado se voc?tem todos os itens de que precisa, e se seu invent嫫io de equip. est?cheio ou n緌." );
		else self->say( "恭喜！完成物品：" .. makeItem .. ". Voc?n緌 acha que eu sou t緌 bom quanto o Sr. Trov緌? Voc?vai ficar mais que feliz com o que eu fiz aqui." );
	}
}

function chat_message5(  index,  makeItem,  needItem,  needNumber,  itemNumber ) {
	
	nRetNum = self->askNumber( "Com #b" .. needNumber .. " " .. needItem .. "#k, eu posso criar" .. itemNumber .. " " .. makeItem .. ". Fique feliz, pois essa ?por minha conta. O que voc?acha? Quantos voc?quer?", 0, 0, 100 );
	nNeedNum = nRetNum * needNumber;
	nAllNum = nRetNum * itemNumber;
	nRetBuy = self->askYesNo( "Voc?quer fazer #b" .. makeItem .. "#k " .. nRetNum .. "vezes? Eu vou precisar de  #r" .. nNeedNum .. " " .. needItem .. "#k ent緌." );
	if ( nRetBuy == 0 ) self->say( "身上沒有足夠的材料或者楓幣嗎？那我勸你可以到各個城鎮尋找必須的材料，等到蒐集完成後再回來找我。" );
	else {
		if ( index == 1 ) ret = inventory->exchange( 0, 4000003, -nNeedNum, 4003001, nAllNum );
		else if ( index == 2 ) ret = inventory->exchange( 0, 4000018, -nNeedNum, 4003001, nAllNum );
		else if ( index == 3 ) ret = inventory->exchange( 0, 4011001, -nNeedNum, 4011000, -nNeedNum, 4003000, nAllNum );
		if ( ret != 0 ) self->say( "Por favor, verifique com cuidado se voc?tem todos os itens de que precisa, e se seu invent嫫io de etc. est?cheio ou n緌." );
		else self->say( "恭喜！ Pegue!" .. nAllNum .. " " .. makeItem .. ". Voc?n緌 acha que eu sou t緌 bom quanto o Sr. Trov緌? Voc?vai ficar mais que feliz com o que eu fiz aqui." );
	}
}

-- refining ores of minerals, jewels and items at Perion.
function s_refine_perion2() {
	nRet1 = self->askYesNo( "Eu sou o aprendiz do Sr. Trov緌. Ele est?ficando muito velho e n緌 ?mais o que era... haha! Ah, que droga! Por favor, n緌 v?contar a ele que eu disse isso... bem... eu posso fazer v嫫ios itens especificamente projetados para os guerreiros, ent緌, o que voc?acha? Quer deixar eu fazer?" );
	if ( nRet1 == 0 ) self->say( "*Suspiro*... Meu chefe com certeza vai me falar um monte se eu n緌 fizer as coisas direitinho hoje ... Ah, bem... que droga!" );
	else {
		v1 = self->askMenu( "Certo! A taxa de servi蔞 ser?razo嫛el, ent緌 n緌 se preocupe. O que voc?quer fazer?\r\n#b#L0# Fazer uma luva#l\r\n#L1# Aperfei蔞ar uma luva#l\r\n#L2# Criar materiais#l" );;
		if ( v1 == 0 ) {
			v2 = self->askMenu( "Eu sou o melhor criador de luvas dessa cidade!! Agora... que tipo de luva voc?quer que eu fa蓷?\r\n#L0##b #t1082003##k (等級限制: 10, guerreiro)#l\r\n#L1##b #t1082000##k(等級限制: 15, guerreiro)#l\r\n#L2##b #t1082004##k(等級限制: 20, guerreiro)#l\r\n#L3##b #t1082001##k(等級限制: 25, guerreiro)#l\r\n#L4##b #t1082007##k(等級限制: 30, guerreiro)#l\r\n#L5##b #t1082008##k(等級限制: 35, guerreiro)#l\r\n#L6##b #t1082023##k(等級限制: 40, guerreiro)#l\r\n#L7##b #t1082009##k(等級限制: 50, guerreiro)#l\r\n#L8##b #t1082059##k(等級限制: 60, guerreiro)#l" );
			if ( v2 == 0 ) chat_message3( 1, "#t1082003#", "#v4000021# 15 #t4000021#\r\n#v4011001# #t4011001# \r\n#v4031138# 1.000 楓幣", 10 );
			else if ( v2 == 1 ) chat_message3( 2, "#t1082000#", "#v4011001# 2 #t4011001# \r\n#v4031138# 2.000 楓幣", 15 );
			else if ( v2 == 2 ) chat_message3( 3, "#t1082004#", "#v4000021# 40 #t4000021# \r\n#v4011000# 2 #t4011000# \r\n#v4031138# 5.000 楓幣", 20 );
			else if ( v2 == 3 ) chat_message3( 4, "#t1082001#", "#v4011001# 2 #t4011001# \r\n#v4031138# 10.000 楓幣", 25 );
			else if ( v2 == 4 ) chat_message3( 5, "#t1082007#", "#v4011000# 3 #t4011000# \r\n#v4011001# 2 #t4011001# \r\n#v4003000# 15 #t4003000# \r\n#v4031138# 20.000 楓幣", 30 );
			else if ( v2 == 5 ) chat_message3( 6, "#t1082008#", "#v4000021# 30 #t4000021# \r\n#v4011001#  4 #t4011001# \r\n#v4003000# 30 #t4003000# \r\n#v4031138# 30.000 楓幣", 35 );
			else if ( v2 == 6 ) chat_message3( 7, "#t1082023#", "#v4000021# 50 #t4000021# \r\n#v4011001# 5 #t4011001# \r\n#v4003000# 40 #t4003000# \r\n#v4031138# 40.000 楓幣", 40 );
			else if ( v2 == 7 ) chat_message3( 8, "#t1082009#", "#v4011001# 3 #t4011001# \r\n#v4021007# 2 #t4021007# \r\n#v4000030# 30 #t4000030# \r\n#v4003000# 45 #t4003000# \r\n#v4031138# 50.000 楓幣", 50 );
			else if ( v2 == 8 ) chat_message3( 9, "#t1082059#", "#v4011007# #t4011007# \r\n#v4011000# 8 #t4011000# \r\n#v4011006# 2 #t4011006# \r\n#v4000030# 50 #t4000030# \r\n#v4003000# 50 #t4003000# \r\n#v4031138# 70.000 楓幣", 60 );
		}
		else if ( v1 == 1 ) {
			self->say( "Ent緌, voc?quer aperfei蔞ar a luva? Ok, ent緌. Mas vou te dar um conselho: Todos os itens que ser緌 usados para o aperfei蔞amento ir緌 desaparecer, e se voc?usar um item que j?tenha sido #raperfei蔞ado#k com um pergaminho, o efeito ir?desaparecer quando aperfei蔞ado. Leve isso em considera誽o quando for tomar a decis緌, certo?" );
			v2 = self->askMenu( "Ent緌~~ que tipo de luva voc?quer aperfei蔞ar e criar?\r\n#L0##b #t1082005##k(等級限制: 30, guerreiro)#l\r\n#L1##b #t1082006##k(等級限制: 30, guerreiro)#l\r\n#L2##b #t1082035##k(等級限制: 35, guerreiro)#l\r\n#L3##b #t1082036##k(等級限制: 35, guerreiro)#l\r\n#L4##b #t1082024##k(等級限制: 40, guerreiro)#l\r\n#L5##b #t1082025##k(等級限制: 40, guerreiro)#l\r\n#L6##b #t1082010##k(等級限制: 50, guerreiro)#l\r\n#L7##b #t1082011##k(等級限制: 50, guerreiro)#l\r\n#L8##b #t1082060##k(等級限制: 60, guerreiro)#l\r\n#L9##b #t1082061##k(等級限制: 60, guerreiro)#l" );
			if ( v2 == 0 ) chat_message4( 1, "#t1082005#", "#v1082007# #t1082007# \r\n#v4011001# #t4011001# \r\n#v4031138# 20.000 楓幣", 30 );
			else if ( v2 == 1 ) chat_message4( 2, "#t1082006#", "#v1082007# #t1082007# \r\n#v4011005# 2 #t4011005# \r\n#v4031138# 25.000 楓幣", 30 );
			else if ( v2 == 2 ) chat_message4( 3, "#t1082035#", "#v1082008# #t1082008# \r\n#v4021006# 3 #t4021006# \r\n#v4031138# 30.000 楓幣", 35 );
			else if ( v2 == 3 ) chat_message4( 4, "#t1082036#", "#v1082008# #t1082008# \r\n#v4021008# #t4021008# \r\n#v4031138# 40.000 楓幣", 35 );
			else if ( v2 == 4 ) chat_message4( 5, "#t1082024#", "#v1082023# #t1082023# \r\n#v4011003# 4 #t4011003# \r\n#v4031138# 45.000 楓幣", 40 );
			else if ( v2 == 5 ) chat_message4( 6, "#t1082025#", "#v1082023# #t1082023# \r\n#v4021008# 2 #t4021008# \r\n#v4031138# 50.000 楓幣", 40 );
			else if ( v2 == 6 ) chat_message4( 7, "#t1082010#", "#v1082009# #t1082009# \r\n#v4011002# 5 #t4011002# \r\n#v4031138# 55.000 楓幣", 50 );
			else if ( v2 == 7 ) chat_message4( 8, "#t1082011#", "#v1082009# #t1082009# \r\n#v4011006# 4 #t4011006# \r\n#v4031138# 60.000 楓幣", 50 );
			else if ( v2 == 8 ) chat_message4( 9, "#t1082060#", "#v1082059# #t1082059# \r\n#v4011002# 3 #t4011002# \r\n#v4021005# 5 #t4021005# \r\n#v4031138# 70.000 楓幣", 60 );
			else if ( v2 == 9 ) chat_message4( 10, "#t1082061#", "#v1082059# #t1082059# \r\n#v4021007# 2 #t4021007# \r\n#v4021008# 2 #t4021008# \r\n#v4031138# 80.000 楓幣", 60 );
		}
		else if ( v1 == 2 ) {
			v2 = self->askMenu( "Ent緌, voc?quer criar alguns materiais, certo? Ok... Que tipos de materiais voc?quer criar?\r\n#L0##bCriar #t4003001# com #t4000003##k#l\r\n#L1##bCriar #t4003001# com #t4000018##k#l\r\n#L2##bCriar #t4003000##k#l" );
			if ( v2 == 0 ) chat_message5( 1, "#t4003001#(s)", "#t4000003#es", 10, 1 );
			else if ( v2 == 1 ) chat_message5( 2, "#t4003001#(s)", "#t4000018#", 5, 1 );
			else if ( v2 == 2 ) chat_message5( 3, "#t4003000#", "#t4011001#(s) e #t4011000#(s) cada", 1, 15 );
		}
	}
}

function chat_message6(  index,  makeItem,  needItem,  reqLevel ) {
	
	nRet = self->askYesNo( "想要製作 " .. makeItem .. " 你必須有足夠的材料。除此之外，還必須達到等級" .. reqLevel .. "等。以下是製作物品的材料清單：\r\n\r\n#b" .. needItem );
	if ( nRet == 0 ) self->say( "身上沒有足夠的材料或者楓幣嗎？那我勸你可以到各個城鎮尋找必須的材料，等到蒐集完成後再回來找我。" );
	else {
		-- Making a claw
		if ( index == 1 ) ret = inventory->exchange( -2000, 4011001, -1, 4000021, -20, 4003000, -5, 1472001, 1 );
		else if ( index == 2 ) ret = inventory->exchange( -3000, 4011000, -2, 4011001, -1, 4000021, -30, 4003000, -10, 1472004, 1 );
		else if ( index == 3 ) ret = inventory->exchange( -15000, 4011000, -3, 4011001, -2, 4000021, -50, 4003000, -20, 1472008, 1 );
		else if ( index == 4 ) ret = inventory->exchange( -30000, 4011000, -4, 4011001, -3, 4000021, -80, 4003000, -25, 1472011, 1 );
		else if ( index == 5 ) ret = inventory->exchange( -40000, 4011000, -3, 4011001, -4, 4000021, -100, 4003000, -30, 1472014, 1 );
		else if ( index == 6 ) ret = inventory->exchange( -50000, 4011000, -4, 4011001, -5, 4000030, -40, 4003000, -35, 1472018, 1 );
		-- Making a glove
		else if ( index == 100 ) ret = inventory->exchange( -1000, 4000021, -15, 1082002, 1 );
		else if ( index == 101 ) ret = inventory->exchange( -7000, 4000021, -30, 4000018, -20, 1082029, 1 );
		else if ( index == 102 ) ret = inventory->exchange( -7000, 4000021, -30, 4000015, -20, 1082030, 1 );
		else if ( index == 103 ) ret = inventory->exchange( -7000, 4000021, -30, 4000020, -20, 1082031, 1 );
		else if ( index == 104 ) ret = inventory->exchange( -10000, 4011000, -2, 4000021, -40, 1082032, 1 );
		else if ( index == 105 ) ret = inventory->exchange( -15000, 4011000, -2, 4011001, -1, 4000021, -10, 1082037, 1 );
		else if ( index == 106 ) ret = inventory->exchange( -25000, 4011001, -2, 4000021, -50, 4003000, -10, 1082042, 1 );
		else if ( index == 107 ) ret = inventory->exchange( -30000, 4011001, -3, 4011000, -1, 4000021, -60, 4003000, -15, 1082046, 1 );
		else if ( index == 108 ) ret = inventory->exchange( -40000, 4000021, -80, 4021000, -3, 4000014, -200, 4003000, -30, 1082075, 1 );
		else if ( index == 109 ) ret = inventory->exchange( -50000, 4021005, -3, 4021008, -1, 4000030, -40, 4003000, -30, 1082065, 1 );
		else if ( index == 110 ) ret = inventory->exchange( -70000, 4011007, -1, 4011000, -8, 4021007, -1, 4000030, -50, 4003000, -50, 1082092, 1 );

		if ( ret != 0 ) self->say( "看來你身上沒有足夠的材料或者金錢，也有可能是你的背包空間已經滿了。" );
		else self->say( "恭喜!完成物品：" .. makeItem .. "。看看它們是不是非常的美麗？如果還有需要，可以再回來找我！" );
	}
}

function chat_message7(  index,  makeItem,  needItem ) {
	
	nRet = self->askYesNo( "想要升級為" .. makeItem .. "，你必須有足夠的材料。再次提醒，請確保您使用的是不曾升級過的物品，否則可能會導致升級失敗。以下是製作物品的材料清單：\r\n\r\n#b" .. needItem );
	if ( nRet == 0 ) self->say( "身上沒有足夠的材料或者楓幣嗎？那我勸你可以到各個城鎮尋找必須的材料，等到蒐集完成後再回來找我。" );
	else {
		-- Upgrading a claw
		if ( index == 1 ) ret = inventory->exchange( -1000, 1472001, -1, 4011002, -1, 1472002, 1 );
		else if ( index == 2 ) ret = inventory->exchange( -2000 , 1472001, -1, 4011006, -1, 1472003, 1 );
		else if ( index == 3 ) ret = inventory->exchange( -3000 , 1472004, -1, 4011001, -2, 1472005, 1 );
		else if ( index == 4 ) ret = inventory->exchange( -5000 , 1472004, -1, 4011003, -2, 1472006, 1 );
		else if ( index == 5 ) ret = inventory->exchange( -5000 , 1472000, -1, 4011001, -3, 4000021, -20, 4003001, -30, 1472007, 1 );
		else if ( index == 6 ) ret = inventory->exchange( -10000 , 1472008, -1, 4011002, -3, 1472009, 1 );
		else if ( index == 7 ) ret = inventory->exchange( -15000 , 1472008, -1, 4011003, -3, 1472010, 1 );
		else if ( index == 8 ) ret = inventory->exchange( -20000 , 1472011, -1, 4011004, -4, 1472012, 1 );
		else if ( index == 9 ) ret = inventory->exchange( -25000 , 1472011, -1, 4021008, -1, 1472013, 1 );
		-- Upgrading a glove
		else if ( index == 100 ) ret = inventory->exchange( -5000, 1082032, -1, 4011002, -1, 1082033, 1 );
		else if ( index == 101 ) ret = inventory->exchange( -7000, 1082032, -1, 4021004, -1, 1082034, 1 );
		else if ( index == 102 ) ret = inventory->exchange( -10000, 1082037, -1, 4011002, -2, 1082038, 1 );
		else if ( index == 103 ) ret = inventory->exchange( -12000, 1082037, -1, 4021004, -2, 1082039, 1 );
		else if ( index == 104 ) ret = inventory->exchange( -15000, 1082042, -1, 4011004, -2, 1082043, 1 );
		else if ( index == 105 ) ret = inventory->exchange( -20000, 1082042, -1, 4011006, -1, 1082044, 1 );
		else if ( index == 106 ) ret = inventory->exchange( -22000, 1082046, -1, 4011005, -3, 1082047, 1 );
		else if ( index == 107 ) ret = inventory->exchange( -25000, 1082046, -1, 4011006, -2, 1082045, 1 );
		else if ( index == 108 ) ret = inventory->exchange( -45000, 1082075, -1, 4011006, -4, 1082076, 1 );
		else if ( index == 109 ) ret = inventory->exchange( -50000, 1082075, -1, 4021008, -2, 1082074, 1 );
		else if ( index == 110 ) ret = inventory->exchange( -55000, 1082065, -1, 4021000, -5, 1082067, 1 );
		else if ( index == 111 ) ret = inventory->exchange( -60000, 1082065, -1, 4011006, -2, 4021008, -1, 1082066, 1 );
		else if ( index == 112 ) ret = inventory->exchange( -70000, 1082092, -1, 4011001, -7, 4000014, -200, 1082093, 1 );
		else if ( index == 113 ) ret = inventory->exchange( -80000, 1082092, -1, 4011006, -7, 4000027, -150, 1082094, 1 );

		if ( ret != 0 ) self->say( "看來你身上沒有足夠的材料或者金錢，也有可能是你的背包空間已經滿了。" );
		else self->say( "恭喜！完成物品：" .. makeItem .. "。看看它們是不是非常的美麗？如果還有需要，可以再回來找我！" );
	}
}

function chat_message8(  index,  makeItem,  needItem,  itemOption ) {
	
	nRet = self->askYesNo( "想要升級為" .. makeItem .. "，你必須有足夠的材料。這個增強過的道具具備了#r" .. itemOption .. "#k的屬性。再次提醒，請確保您使用的是不曾升級過的物品，否則可能會導致升級失敗。以下是製作物品的材料清單：\r\n\r\n#b" .. needItem );
	if ( nRet == 0 ) self->say( "身上沒有足夠的材料或者楓幣嗎？那我勸你可以到各個城鎮尋找必須的材料，等到蒐集完成後再回來找我。" );
	else {
		if ( index == 1 ) ret = inventory->exchange( -30000 , 1472014, -1, 4021000, -5, 1472015, 1 );
		else if ( index == 2 ) ret = inventory->exchange( -30000 , 1472014, -1, 4011003, -5, 1472016, 1 );
		else if ( index == 3 ) ret = inventory->exchange( -35000 , 1472014, -1, 4021008, -2, 1472017, 1 );
		else if ( index == 4 ) ret = inventory->exchange( -40000 , 1472018, -1, 4021000, -6, 1472019, 1 );
		else if ( index == 5 ) ret = inventory->exchange( -40000 , 1472018, -1, 4021005, -6, 1472020, 1 );

		if ( ret != 0 ) self->say( "看來你身上沒有足夠的材料或者金錢，也有可能是你的背包空間已經滿了。" );
		else self->say( "恭喜！完成物品：" .. makeItem .. "。看看它們是不是非常的美麗？如果還有需要，可以再回來找我！" );
	}
}

function chat_message9(  index,  makeItem,  needItem,  needNumber,  itemNumber ) {
	
	nRetNum = self->askNumber( "每#b" .. needNumber .. "個" .. needItem .. "#k可以製作" .. itemNumber .. "個" .. makeItem .. "。你可以自由決定要製作多少個，你覺得呢，你想要一次製作幾個？", 0, 0, 100 );
	nNeedNum = nRetNum * needNumber;
	nAllNum = nRetNum * itemNumber;
	nRetBuy = self->askYesNo( "想要製作 ".. nRetNum .."個#b" .. makeItem .. "？這將需要#r" .. nNeedNum .. "個" .. needItem .. "#k來製作。" );
	if ( nRetBuy == 0 ) self->say( "身上沒有足夠的材料或者楓幣嗎？那我勸你可以到各個城鎮尋找必須的材料，等到蒐集完成後再回來找我。" );
	else {
		if ( index == 1 ) ret = inventory->exchange( 0, 4000003, -nNeedNum, 4003001, nAllNum );
		else if ( index == 2 ) ret = inventory->exchange( 0, 4000018, -nNeedNum, 4003001, nAllNum );
		else if ( index == 3 ) ret = inventory->exchange( 0, 4011001, -nNeedNum, 4011000, -nNeedNum, 4003000, nAllNum );
		if ( ret != 0 ) self->say(  "看來你身上沒有足夠的材料或者金錢，也有可能是你的背包空間已經滿了。" );
		else self->say( "恭喜！製作了" .. nAllNum .. "個" .. makeItem .. "。看看它們是不是非常的美麗？如果還有需要，可以再回來找我！" );
	}
}

--creating and upgrading items at kerning city
function s_refine_kerning() {
	nRet1 = self->askYesNo( "你身上有任何的#t4000021#嗎？我可以為您製作完美的裝備，如果你有足夠的數量的話...對了，千萬別告訴別人我在這裡的生意。如何，想要進行交換嗎？" );
	if ( nRet1 == 0 ) self->say( "真的嗎？對我來說一點都無所謂，但是你真的不會後悔嗎？如果你改變主意，隨時可以回來找我。" );
	else {
		v1 = self->askMenu( "在我這裡一切的費用都是合理的，這點你不用擔心，那麼，你想要製作什麼呢？\r\n#b#L0# 製作拳套#l\r\n#L1# 製作手套#l\r\n#L2# 升級拳套#l\r\n#L3# 升級手套#l\r\n#L4# 製作材料#l" );;
		if ( v1 == 0 ) {
			v2 = self->askMenu( "想製作拳套？拳套對於盜賊來說是非常重要的，但是並不是一般人都可以發揮自如。你想要讓我製作哪一種拳套呢？\r\n#L0##b #t1472001##k(等級限制: 15 等)#l\r\n#L1##b #t1472004##k(等級限制: 20 等)#l\r\n#L2##b #t1472008##k(等級限制: 30 等)#l\r\n#L3##b #t1472011##k(等級限制: 35 等)#l\r\n#L4##b #t1472014##k(等級限制: 40 等)#l\r\n#L5##b #t1472018##k(等級限制: 50 等)#l" );
			if ( v2 == 0 ) chat_message6( 1, "#t1472001#", "#v4011001# #t4011001# \r\n#v4000021# 20 #t4000021# \r\n#v4003000# 5 #t4003000# \r\n#v4031138# 2000 楓幣", 15 );
			else if ( v2 == 1 ) chat_message6( 2, "#t1472004#", "#v4011000# 2 #t4011000# \r\n#v4011001# #t4011001# \r\n#v4000021# 30 #t4000021# \r\n#v4003000# 10 #t4003000# \r\n#v4031138# 3000 楓幣", 20 );
			else if ( v2 == 2 ) chat_message6( 3, "#t1472008#", "#v4011000# 3 #t4011000# \r\n#v4011001# 2 #t4011001# \r\n#v4000021# 50 #t4000021# \r\n#v4003000# 20 #t4003000# \r\n#v4031138# 15000 楓幣", 30 );
			else if ( v2 == 3 ) chat_message6( 4, "#t1472011#", "#v4011000# 4 #t4011000# \r\n#v4011001# 3 #t4011001# \r\n#v4000021# 80#t4000021# \r\n#v4003000# 25 #t4003000# \r\n#v4031138# 30000 楓幣", 35 );
			else if ( v2 == 4 ) chat_message6( 5, "#t1472014#", "#v4011000# 3 #t4011000# \r\n#v4011001# 4 #t4011001# \r\n#v4000021# 100 #t4000021# \r\n#v4003000# 30 #t4003000# \r\n#v4031138# 40000 楓幣", 40 );
			else if ( v2 == 5 ) chat_message6( 6, "#t1472018#", "#v4011000# 4 #t4011000# \r\n#v4011001# 5 #t4011001# \r\n#v4000030# 40 #t4000030# \r\n#v4003000# 35 #t4003000# \r\n#v4031138# 50000 楓幣", 50 );
		}
		else if ( v1 == 1 ) {
			v2 = self->askMenu( "手套...這裡有好幾種選擇，哪一個是你喜歡的呢？\r\n#L0##b #t1082002##k(等級限制: 10 等)#l\r\n#L1##b #t1082029##k(等級限制: 15 等)#l\r\n#L2##b #t1082030##k(等級限制: 15 等)#l\r\n#L3##b #t1082031##k(等級限制: 15 等)#l\r\n#L4##b #t1082032##k(等級限制: 20 等)#l\r\n#L5##b #t1082037##k(等級限制: 25 等)#l\r\n#L6##b #t1082042##k(等級限制: 30 等)#l\r\n#L7##b #t1082046##k(等級限制: 35 等)#l\r\n#L8##b #t1082075##k(等級限制: 40 等)#l\r\n#L9##b #t1082065##k(等級限制: 50 等)#l\r\n#L10##b #t1082092##k(等級限制: 60 等)#l" );
			if ( v2 == 0 ) chat_message6( 100, "#t1082002#", "#v4000021# 15 #t4000021# \r\n#v4031138# 1.000 楓幣", 10 );
			else if ( v2 == 1 ) chat_message6( 101, "#t1082029#", "#v4000021# 30 #t4000021# \r\n#v4000018# 20 #t4000018# \r\n#v4031138# 7.000 楓幣", 15 );
			else if ( v2 == 2 ) chat_message6( 102, "#t1082030#", "#v4000021# 30 #t4000021# \r\n#v4000015# 20 #t4000015# \r\n#v4031138# 7.000 楓幣", 15 );
			else if ( v2 == 3 ) chat_message6( 103, "#t1082031#", "#v4000021# 30 #t4000021# \r\n#v4000020# 20 #t4000020# \r\n#v4031138# 7.000 楓幣", 15 );
			else if ( v2 == 4 ) chat_message6( 104, "#t1082032#", "#v4011000# 2 #t4011000# \r\n#v4000021# 40 #t4000021# \r\n#v4031138# 10.000 楓幣", 20 );
			else if ( v2 == 5 ) chat_message6( 105, "#t1082037#", "#v4011000# 2 #t4011000# \r\n#v4011001# #t4011001# \r\n#v4000021# 10 #t4000021# \r\n#v4031138# 15.000 楓幣", 25 );
			else if ( v2 == 6 ) chat_message6( 106, "#t1082042#", "#v4011001# 2 #t4011001# \r\n#v4000021# 50 #t4000021# \r\n#v4003000# 10 #t4003000# \r\n#v4031138# 25.000 楓幣", 30 );
			else if ( v2 == 7 ) chat_message6( 107, "#t1082046#", "#v4011001# 3 #t4011001# \r\n#v4011000# #t4011000# \r\n#v4000021# 60 #t4000021# \r\n#v4003000# 15 #t4003000# \r\n#v4031138# 30.000 楓幣", 35 );
			else if ( v2 == 8 ) chat_message6( 108, "#t1082075#", "#v4021000# 3 #t4021000# \r\n#v4000014# 200 #t4000014# \r\n#v4000021# 80 #t4000021# \r\n#v4003000# 30 #t4003000# \r\n#v4031138# 40.000 楓幣", 40 );
			else if ( v2 == 9 ) chat_message6( 109, "#t1082065#", "#v4021005# 3 #t4021005# \r\n#v4021008# #t4021008# \r\n#v4000030# 40 #t4000030# \r\n#v4003000# 30 #t4003000# \r\n#v4031138# 50.000 楓幣", 50 );
			else if ( v2 == 10 ) chat_message6( 110, "#t1082092#", "#v4011007# #t4011007# \r\n#v4011000# 8 #t4011000# \r\n#v4021007# #t4021007# \r\n#v4000030# 50 #t4000030# \r\n#v4003000# 50 #t4003000# \r\n#v4031138# 70.000 楓幣", 60 );
		}
		else if ( v1 == 2 ) {
			self->say( "你想要升級拳套嗎？這難不倒我，但我還是要提醒你一點，所有用於升級的道具都會被消耗殆盡，而且如果使用的道具已經被升級了，那就無法用於其他升級用途。" );
			v2 = self->askMenu( "我提供了各種選擇，你想要升級為哪一個？\r\n#L0##b #t1472002##k(等級限制: 15 等)#l\r\n#L1##b #t1472003##k(等級限制: 15 等)#l\r\n#L2##b #t1472005##k(等級限制: 20 等)#k#l\r\n#L3##b #t1472006##k(等級限制: 20 等)#l\r\n#L4##b #t1472007##k(等級限制: 25 等)#l\r\n#L5##b #t1472009##k(等級限制: 30 等)#l\r\n#L6##b #t1472010##k(等級限制: 30 等)#l\r\n#L7##b #t1472012##k(等級限制: 35 等)#l\r\n#L8##b #t1472013##k(等級限制: 35 等)#l\r\n#L9##b #t1472015##k(等級限制: 40 等)#l\r\n#L10##b #t1472016##k(等級限制: 40 等)#l\r\n#L11##b #t1472017##k(等級限制: 40 等)#l\r\n#L12##b #t1472019##k(等級限制: 50 等)#l\r\n#L13##b #t4021005# 拳套#k(等級限制: 50 等)#l" );
			if ( v2 == 0 ) chat_message7( 1, "#t1472002#", "#v1472001# #t1472001# \r\n#v4011002# #t4011002# \r\n#v4031138# 1.000 楓幣" );
			else if ( v2 == 1 ) chat_message7( 2, "#t1472003#", "#v1472001# #t1472001# \r\n#v4011006# #t4011006# \r\n#v4031138# 2.000 楓幣" );
			else if ( v2 == 2 ) chat_message7( 3, "#t1472005#", "#v1472004# #t1472004# \r\n#v4011001# 2 #t4011001# \r\n#v4031138# 3.000 楓幣" );
			else if ( v2 == 3 ) chat_message7( 4, "#t1472006#", "#v1472004# #t1472004# \r\n#v4011003# 2 #t4011003# \r\n#v4031138# 5.000 楓幣" );
			else if ( v2 == 4 ) chat_message7( 5, "#t1472007#", "#v1472000# #t1472000# \r\n#v4011001# 3 #t4011001# \r\n#v4000021# 20 #t4000021# \r\n#v4003001# 30 #t4003001# \r\n#v4031138# 5.000 楓幣" );
			else if ( v2 == 5 ) chat_message7( 6, "#t1472009#", "#v1472008# #t1472008# \r\n#v4011002# 3 #t4011002# \r\n#v4031138# 10.000 楓幣" );
			else if ( v2 == 6 ) chat_message7( 7, "#t1472010#", "#v1472008# #t1472008# \r\n#v4011003# 3 #t4011003# \r\n#v4031138# 15.000 楓幣" );
			else if ( v2 == 7 ) chat_message7( 8, "#t1472012#", "#v1472011# #t1472011# \r\n#v4011004# 4 #t4011004# \r\n#v4031138# 20.000 楓幣" );
			else if ( v2 == 8 ) chat_message7( 9, "#t1472013#", "#v1472011# #t1472011# \r\n#v4021008# #t4021008# \r\n#v4031138# 25.000 楓幣" );
			else if ( v2 == 9 ) chat_message8( 1, "#t1472015#", "#v1472014# #t1472014# \r\n#v4021000# 5 #t4021000# \r\n#v4031138# 30.000 楓幣", "DEX +2" );
			else if ( v2 == 10 ) chat_message8( 2, "#t1472016#", "#v1472014# #t1472014# \r\n#v4011003# 5 #t4011003# \r\n#v4031138# 30.000 楓幣", "LUK +2" );
			else if ( v2 == 11 ) chat_message8( 3, "#t1472017#", "#v1472014# #t1472014# \r\n#v4021008# 2 #t4021008# \r\n#v4031138# 35.000 楓幣", "LUK +3" );
			else if ( v2 == 12 ) chat_message8( 4, "#t1472019#", "#v1472018# #t1472018# \r\n#v4021000# 6 #t4021000# \r\n#v4031138# 40.000 楓幣", "DEX +3" );
			else if ( v2 == 13 ) chat_message8( 5, "#t1472020#", "#v1472018# #t1472018# \r\n#v4021005# 6 #t4021005# \r\n#v4031138# 40.000 楓幣", "LUK +3" );
		}
		else if ( v1 == 3 ) {
			self->say( "你想要升級手套嗎？這難不倒我，但我還是要提醒你一點，所有用於升級的道具都會被消耗殆盡，而且如果使用的道具已經被升級了，那就無法用於其他升級用途。" );
			v2 = self->askMenu( "所以哪一種手套你想要升級獲得呢？\r\n#L0##b #t1082033##k(等級限制: 20 等)#l\r\n#L1##b #t1082034##k(等級限制: 20 等)#l\r\n#L2##b #t1082038##k(等級限制: 25 等)#l\r\n#L3##b #t1082039##k(等級限制: 25 等)#l\r\n#L4##b #t1082043##k(等級限制: 30 等)#l\r\n#L5##b #t1082044##k(等級限制: 30 等)#l\r\n#L6##b #t1082047##k(等級限制: 35 等)#l\r\n#L7##b #t1082045##k(等級限制: 35 等)#l\r\n#L8##b #t1082076##k(等級限制: 40 等)#l\r\n#L9##b #t1082074##k(等級限制: 40 等)#l\r\n#L10##b #t1082067##k(等級限制: 50 等)#l\r\n#L11##b #t1082066##k(等級限制: 50 等)#l\r\n#L12##b #t1082093##k(等級限制: 60 等)#l\r\n#L13##b #t1082094##k(等級限制: 60 等)#l" );
			if ( v2 == 0 ) chat_message7( 100, "#t1082033#", "#v1082032# #t1082032# \r\n#v4011002# #t4011002# \r\n#v4031138# 5.000 楓幣" );
			else if ( v2 == 1 ) chat_message7( 101, "#t1082034#", "#v1082032# #t1082032# \r\n#v4021004# #t4021004# \r\n#v4031138# 7.000 楓幣" );
			else if ( v2 == 2 ) chat_message7( 102, "#t1082038#", "#v1082037# #t1082037# \r\n#v4011002# 2 #t4011002# \r\n#v4031138# 10.000 楓幣" );
			else if ( v2 == 3 ) chat_message7( 103, "#t1082039#", "#v1082037# #t1082037# \r\n#v4021004# 2 #t4021004# \r\n#v4031138# 12.000 楓幣" );
			else if ( v2 == 4 ) chat_message7( 104, "#t1082043#", "#v1082042# #t1082042# \r\n#v4011004# 2 #t4011004# \r\n#v4031138# 15.000 楓幣" );
			else if ( v2 == 5 ) chat_message7( 105, "#t1082044#", "#v1082042# #t1082042# \r\n#v4011006# #t4011006# \r\n#v4031138# 20.000 楓幣" );
			else if ( v2 == 6 ) chat_message7( 106, "#t1082047#", "#v1082046# #t1082046# \r\n#v4011005# 3 #t4011005# \r\n#v4031138# 22.000 楓幣" );
			else if ( v2 == 7 ) chat_message7( 107, "#t1082045#", "#v1082046# #t1082046# \r\n#v4011006# 2 #t4011006# \r\n#v4031138# 25.000 楓幣" );
			else if ( v2 == 8 ) chat_message7( 108, "#t1082076#", "#v1082075# #t1082075# \r\n#v4011006# 4 #t4011006# \r\n#v4031138# 45.000 楓幣" );
			else if ( v2 == 9 ) chat_message7( 109, "#t1082074#", "#v1082075# #t1082075# \r\n#v4021008# 2 #t4021008# \r\n#v4031138# 50.000 楓幣" );
			else if ( v2 == 10 ) chat_message7( 110, "#t1082067#", "#v1082065# #t1082065# \r\n#v4021000# 5 #t4021000# \r\n#v4031138# 55.000 楓幣" );
			else if ( v2 == 11 ) chat_message7( 111, "#t1082066#", "#v1082065# #t1082065# \r\n#v4011006# 2 #t4011006# \r\n#v4021008# #t4021008# \r\n#v4031138# 60.000 楓幣" );
			else if ( v2 == 12 ) chat_message7( 112, "#t1082093#", "#v1082092# #t1082092# \r\n#v4011001# 7 #t4011001# \r\n#v4000014# 200 #t4000014# \r\n#v4031138# 70.000 楓幣" );
			else if ( v2 == 13 ) chat_message7( 113, "#t1082094#", "#v1082092# #t1082092# \r\n#v4011006# 7 #t4011006# \r\n#v4000027# 150 #t4000027# \r\n#v4031138# 80.000 楓幣" );
		}
		else if ( v1 == 4 ) {
			v2 = self->askMenu( "製作材料？沒問題，這方面我也很在行。\r\n#L0##b 用#t4000003#製作#t4003001##k#l\r\n#L1##b 用#t4000018#製作#t4003001##k#l\r\n#L2##b 製作#t4003000# #k#l" );
			if ( v2 == 0 ) chat_message9( 1, "#t4003001#", "#t4000003#", 10, 1 );
			else if ( v2 == 1 ) chat_message9( 2, "#t4003001#", "#t4000018#", 5, 1 );
			else if ( v2 == 2 ) chat_message9( 3, "#t4003000#", "#t4011001#與#t4011000#", 1, 15 );
		}
	}
}
