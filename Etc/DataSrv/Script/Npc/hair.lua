require "./DataSrv/Script/sysDef"

-- Henesys VIP hair salon 
function s_hair_henesys1() {
-- 	selectHair = self->askMenu( "你好，我是Nat嫮ia, a chefe deste sal緌 de beleza如果你身上有#b#t5150001##k, #b#t5151001##k或#b#t5420002##k, deixe-me cuidar do seu penteado. Escolha o que quer.\r\n#b#L0# 更換髮型 (使用高級美髮券)#l\r\n#L1# 更換髮色 (使用高級會員卡)#l\r\n#L2# Mudar o estilo  (Cupom de s鏂io VIP)#l" ); 
	selectHair = self->askMenu( "歡迎來到弓箭手村美髮店，只要你有攜帶#b#t5150001##k或者#b#t5151001##k我就可以幫你更換你喜歡的造型。請選擇你需要的服務：\r\n#b#L0# 更換髮型 (使用高級美髮券)#l\r\n#L1# 更換髮色 (使用高級會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30480 + tHair; 
			
 			changeHair5 = 30310 + tHair; 
 			changeHair6 = 30330 + tHair; 
 			changeHair7 = 30060 + tHair; 
 			changeHair8 = 30150 + tHair; 
 			changeHair9 = 30410 + tHair; 
 			changeHair10 = 30210 + tHair; 
 			changeHair11 = 30140 + tHair; 
 			changeHair12 = 30120 + tHair; 
 			changeHair13 = 30200 + tHair; 
			
 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150001##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150001, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31050 + tHair; 
 			changeHair2 = 31040 + tHair; 
 			changeHair3 = 31000 + tHair; 
 			changeHair4 = 31700 + tHair; 
			
 			changeHair5 = 31150 + tHair; 
 			changeHair6 = 31310 + tHair; 
 			changeHair7 = 31300 + tHair; 
 			changeHair8 = 31160 + tHair; 
 			changeHair9 = 31100 + tHair; 
 			changeHair10 = 31410 + tHair; 
 			changeHair11 = 31030 + tHair; 
 			changeHair12 = 31080 + tHair; 
 			changeHair13 = 31070 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150001##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150001, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 		} 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，除非你有攜帶所需的會員卡，否則我無法幫你更換造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair1 = eHair; 
 		changeHair2 = eHair + 1; 
 		changeHair4 = eHair + 2; 
 		changeHair6 = eHair + 4; 
 		changeHair3 = eHair + 6; 
 		changeHair5 = eHair + 7; 

 		mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5105100#k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5151001, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，除非你有攜帶所需的會員卡，否則我無法幫你更換造型。" ); 
 	} 
--[[
 	else if ( selectHair == 2 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30480 + tHair; 
			
 			changeHair5 = 30310 + tHair; 
 			changeHair6 = 30330 + tHair; 
 			changeHair7 = 30060 + tHair; 
 			changeHair8 = 30150 + tHair; 
 			changeHair9 = 30410 + tHair; 
 			changeHair10 = 30210 + tHair; 
 			changeHair11 = 30140 + tHair; 
 			changeHair12 = 30120 + tHair; 
 			changeHair13 = 30200 + tHair; 
			
 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420002##k eu mudarei isso para voc? Com este cupom, voc?tem o poder de mudar o estilo do seu cabelo para algo novo, sempre que quiser, por UM M邘! Agora, escolha um estilo que goste.", 5420002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31050 + tHair; 
 			changeHair2 = 31040 + tHair; 
 			changeHair3 = 31000 + tHair; 
 			changeHair4 = 31700 + tHair; 
			
 			changeHair5 = 31150 + tHair; 
 			changeHair6 = 31310 + tHair; 
 			changeHair7 = 31300 + tHair; 
 			changeHair8 = 31160 + tHair; 
 			changeHair9 = 31100 + tHair; 
 			changeHair10 = 31410 + tHair; 
 			changeHair11 = 31030 + tHair; 
 			changeHair12 = 31080 + tHair; 
 			changeHair13 = 31070 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420002##k eu mudarei isso para voc? Com este cupom, voc?tem o poder de mudar o estilo do seu cabelo para algo novo, sempre que quiser, por UM M邘! Agora, escolha um estilo que goste.", 5420002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 		} 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，除非你有攜帶所需的會員卡，否則我無法幫你更換造型。" ); 
 	} 
 ]]--
 } 

-- Henesys EXP hair salon 
function s_hair_henesys2() {
 	selectHair = self->askMenu( "你好，我是這裡的助手，只要你有攜帶#b#t5150000##k或#b#t5151000##k你就可以#b隨機更換造型#k。\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L2# 更換髮色 (使用一般會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		nRet1 = self->askYesNo( "由於我還只是個學徒，使用一般券我只能幫你#b隨機更換造型#k。但不必太擔心，我想我也是非常熟練的！你確定要使用#b#t5150000##k來更換造型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "沒關係...但我想你真的擔心過頭了，我對我自己的實力是非常有自信的！"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 
 				changeHair1 = 30000 + tHair;
				changeHair2 = 30020 + tHair;
				changeHair3 = 30030 + tHair;
				changeHair4 = 30060 + tHair;
				changeHair5 = 30120 + tHair;
				changeHair6 = 30140 + tHair;
				changeHair7 = 30150 + tHair;
				changeHair8 = 30200 + tHair;
				changeHair9 = 30210 + tHair;
				changeHair10 = 30310 + tHair;
				changeHair11 = 30330 + tHair;
				changeHair12 = 30410 + tHair;
								
 				mHair = self->makeRandAvatar( 5150000, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 			} 
 			else if ( target->getGender() == 1 ) { 
 				changeHair1 = 31000 + tHair;
				changeHair2 = 31030 + tHair;
				changeHair3 = 31040 + tHair;
				changeHair4 = 31050 + tHair;
				changeHair5 = 31080 + tHair;
				changeHair6 = 31070 + tHair;
				changeHair7 = 31100 + tHair;
				changeHair8 = 31150 + tHair;
				changeHair9 = 31160 + tHair;
				changeHair10 = 31300 + tHair;
				changeHair11 = 31310 + tHair;
				changeHair12 = 31410 + tHair;
				
 				mHair = self->makeRandAvatar( 5150000, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 			} 
 			if ( mHair == 1 ) self->say( "怎麼樣？是不是非常棒啊？如果你還需要更換造型，隨時都可以回到這裡來。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 		} 
 	} 
	
 	else if ( selectHair == 2 ) { 
 		nRet1 = self->askYesNo( "使用一般的會員卡，我只能幫你#b隨機更換髮色#k，你確定要使用#b#t5151000##k來更換造型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "沒關係...但我想你真的擔心過頭了，我對我自己的實力是非常有自信的！"); 
 		else if ( nRet1 == 1 ) { 
 			cHair = target->getHair(); 
 			eHair = cHair - ( cHair % 10 ); 

 			changeHair1 = eHair; 
 			changeHair2 = eHair + 1; 
 			changeHair4 = eHair + 2; 
 			changeHair6 = eHair + 4; 
 			changeHair3 = eHair + 6; 
 			changeHair5 = eHair + 7; 

 			mHair = self->makeRandAvatar( 5151000, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 			if ( mHair == 1 ) self->say( "完成了！你可以到這邊的鏡子看看新的髮型。怎麼樣，還算滿意對吧？哈哈，如果有需要還可以來找我。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso tingir seu cabelo sem ele. Me desculpe." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你必須攜帶會員卡我才能幫你更換造型。" ); 
 		} 
 	} 
 } 

-- Kerning City VIP hair salon 
function s_hair_kerning1() {
-- 	selectHair = self->askMenu( "歡迎來到墮落城市美髮店，只要你有攜帶#b#t5150003##k, #b#t5151003##k或#b#t5420003##k我就可以幫你更換造型。那麼，需要什麼服務嗎？\r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮型顏色 (使用高級會員卡)#l\r\n#L2# 更換髮型 (使用VIP會員卡 ※#r台版僅供預覽#b※)#l" ); 
	selectHair = self->askMenu( "歡迎來到墮落城市美髮店，只要你有攜帶#b#t5150003##k或者#b#t5151003##k我就可以幫你更換造型。那麼，需要什麼服務嗎？\r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮型顏色 (使用高級會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30780 + tHair; 
			
 			changeHair5 = 30130 + tHair; 
 			changeHair6 = 30350 + tHair; 
 			changeHair7 = 30190 + tHair; 
 			changeHair8 = 30110 + tHair; 
 			changeHair9 = 30180 + tHair; 
 			changeHair10 = 30050 + tHair; 
 			changeHair11 = 30040 + tHair; 
 			changeHair12 = 30160 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150003##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150003, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31050 + tHair; 
 			changeHair2 = 31040 + tHair; 
 			changeHair3 = 31000 + tHair; 
 			changeHair4 = 31760 + tHair; 
			
 			changeHair5 = 31060 + tHair; 
 			changeHair6 = 31090 + tHair; 
 			changeHair7 = 31330 + tHair; 
 			changeHair8 = 31020 + tHair; 
 			changeHair9 = 31130 + tHair; 
 			changeHair10 = 31120 + tHair; 
 			changeHair11 = 31140 + tHair; 
 			changeHair12 = 31010 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150003##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150003, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Desculpe, colega." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair1 = eHair; 
 		changeHair2 = eHair + 2; 
 		changeHair4 = eHair + 3; 
 		changeHair3 = eHair + 7; 
 		changeHair5 = eHair + 5; 

 		mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5151003##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5151003, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5 ); 

 		if ( mHair == 1 ) self->say( "怎麼樣？是不是非常棒啊？如果你還需要更換造型，隨時都可以回到這裡來。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso tingir seu cabelo sem ele. Desculpe, colega." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Volte mais tarde." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
--[[
 	else if ( selectHair == 2 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30780 + tHair; 
			
 			changeHair5 = 30130 + tHair; 
 			changeHair6 = 30350 + tHair; 
 			changeHair7 = 30190 + tHair; 
 			changeHair8 = 30110 + tHair; 
 			changeHair9 = 30180 + tHair; 
 			changeHair10 = 30050 + tHair; 
 			changeHair11 = 30040 + tHair; 
 			changeHair12 = 30160 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420003##k我就可以幫你把髮型改造成最新的樣式，而且保證一定會讓你非常滿意的！", 5420003, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31050 + tHair; 
 			changeHair2 = 31040 + tHair; 
 			changeHair3 = 31000 + tHair; 
 			changeHair4 = 31760 + tHair; 
			
 			changeHair5 = 31060 + tHair; 
 			changeHair6 = 31090 + tHair; 
 			changeHair7 = 31330 + tHair; 
 			changeHair8 = 31020 + tHair; 
 			changeHair9 = 31130 + tHair; 
 			changeHair10 = 31120 + tHair; 
 			changeHair11 = 31140 + tHair; 
 			changeHair12 = 31010 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420003##k我就可以幫你把髮型改造成最新的樣式，而且保證一定會讓你非常滿意的！", 5420003, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Desculpe, colega." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 ]]--
 } 



-- Kerning City EXP hair salon 
function s_hair_kerning2() {
-- 	selectHair = self->askMenu( "歡迎光臨，我是這裡的助手，只要你有攜帶#b#t5150011##k. #b#t5150002##k或#b#t5151002##k就可以更改你的造型。\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L2# Pintar seu cabelo(cupom REG)#l" ); 
	selectHair = self->askMenu( "歡迎光臨，我是這裡的助手，只要你有攜帶#b#t5150002##k或#b#t5151002##k就可以更改你的造型。\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L2# 更換髮色 (使用一般會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		--nRet1 = self->askYesNo( "Se usar o cupom EXP, seu cabelo vai mudar ALEATORIAMENTE com a chance de ganhar um novo estilo experimental criado por mim. Vai usar #b#t5150002##k來更換造型嗎？" ); 
		nRet1 = self->askYesNo( "由於我還只是個學徒，使用一般券我只能幫你#b隨機更換造型#k。但不必太擔心，我想我也是非常熟練的！你確定要使用#b#t5150002##k來更換造型嗎？" );
 		if ( nRet1 == 0 ) self->say( "沒問題...等你有需要的時候可以回來找我。"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 
--[[
 				changeHair1 = 30520 + tHair; 
 				changeHair2 = 30620 + tHair; 
 				changeHair3 = 30770 + tHair; 
				
 				changeHair4 = 30130 + tHair; 
 				changeHair5 = 30350 + tHair; 
 				changeHair6 = 30190 + tHair; 
 				changeHair7 = 30110 + tHair; 
 				changeHair8 = 30180 + tHair; 
 				changeHair9 = 30050 + tHair; 
 				changeHair10 = 30040 + tHair; 
 				changeHair11 = 30160 + tHair; 
 				changeHair12 = 30550 + tHair; 
								
 				mHair = self->makeRandAvatar( 5150002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 ]]--
				changeHair1 = 30000 + tHair;
				changeHair2 = 30020 + tHair;
				changeHair3 = 30030 + tHair;
				changeHair4 = 30040 + tHair;
				changeHair5 = 30050 + tHair;
				changeHair6 = 30110 + tHair;
				changeHair7 = 30130 + tHair;
				changeHair8 = 30160 + tHair;
				changeHair9 = 30180 + tHair;
				changeHair10 = 30190 + tHair;
				changeHair11 = 30350 + tHair;
				changeHair12 = 30610 + tHair;
				changeHair13 = 30440 + tHair;
				changeHair14 = 30400 + tHair;	
				
				mHair = self->makeRandAvatar( 5150002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13, changeHair14 );
 			} 
 			else if ( target->getGender() == 1 ) { 
--[[
 				changeHair1 = 31520 + tHair; 
 				changeHair2 = 31620 + tHair; 
 				changeHair3 = 31750 + tHair; 
				
 				changeHair4 = 31060 + tHair; 
 				changeHair5 = 31090 + tHair; 
 				changeHair6 = 31330 + tHair; 
 				changeHair7 = 31020 + tHair; 
 				changeHair8 = 31130 + tHair; 
 				changeHair9 = 31120 + tHair; 
 				changeHair10 = 31140 + tHair; 
 				changeHair11 = 31010 + tHair;	 
 				changeHair12 = 31440 + tHair;		--Black Ravishing Raven			 

 				mHair = self->makeRandAvatar( 5150011, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 ]]--
				changeHair1 = 31000 + tHair;
				changeHair2 = 31010 + tHair;
				changeHair3 = 31020 + tHair;
				changeHair4 = 31040 + tHair;
				changeHair5 = 31050 + tHair;
				changeHair6 = 31060 + tHair;
				changeHair7 = 31090 + tHair;
				changeHair8 = 31120 + tHair;
				changeHair9 = 31130 + tHair;
				changeHair10 = 31140 + tHair;
				changeHair11 = 31330 + tHair;
				changeHair12 = 31700 + tHair;
				changeHair13 = 31620 + tHair;
				changeHair14 = 31610 + tHair;
				
				mHair = self->makeRandAvatar( 5150002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13, changeHair14 );
 			} 
 			if ( mHair == 1 ) self->say( "怎麼樣？這個造型非常棒對吧！現在你也是時尚玩家了。如果還想更換造型，歡迎再度光臨。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Tem certeza de que tem o cupom certo? Desculpe, mas nada de corte de cabelo sem ele." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 		} 
 	} 
	
 	else if ( selectHair == 2 ) { 
 		nRet1 = self->askYesNo( "使用一般的會員卡，我只能幫你#b隨機更換髮色#k，你確定要使用#b#t5151002##k來進行染髮嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "沒問題...等你有需要的時候可以回來找我。"); 
 		else if ( nRet1 == 1 ) { 
 			cHair = target->getHair(); 
 			eHair = cHair - ( cHair % 10 ); 

 			changeHair1 = eHair; 
 			changeHair2 = eHair + 2; 
 			changeHair4 = eHair + 3; 
 			changeHair3 = eHair + 7; 
 			changeHair5 = eHair + 5; 

 			if ( target->getGender() == 0 ) mHair = self->makeRandAvatar( 5151002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5 ); 
 			else if ( target->getGender() == 1 ) mHair = self->makeRandAvatar( 5151002, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5 ); 

 			if ( mHair == 1 ) self->say( "怎麼樣？這個造型非常棒對吧！現在你也是時尚玩家了。如果還想更換造型，歡迎再度光臨。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Tem certeza de que tem o cupom certo? Desculpe, mas nada de corte de cabelo sem ele." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Volte mais tarde." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你必須攜帶會員卡我才能幫你更換造型。" ); 
 		} 
 	} 
 } 

-- Orbis VIP hair salon 
function s_hair_orbis1() {
-- 	selectHair = self->askMenu( "嗨！我是#p2010001#如果你身上有#b#t5150005##k, #b#t5151005##k或#b#t5420004##k那我可以幫你的頭髮變更成你所喜歡的髮型樣式。\r\n#b#L0# 更換髮型 (使用高級美髮券)#l\r\n#L1# 更換髮型顏色 (使用高級會員卡)#l\r\n#L2# Usar #t5154000##l\r\n#L3# Mudar estilo (cupom de membro VIP)#l" ); 
	selectHair = self->askMenu( "嗨！我是#p2010001#如果你身上有#b#t5150005##k或#b#t5151005##k，我就可以幫你的頭髮變更成你所喜歡的髮型樣式。\r\n#b#L0# 更換髮型 (使用高級美髮券)#l\r\n#L1# 更換髮型顏色 (使用高級會員卡)#l\r\n#L2# 使用#t5154000##l\r\n" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30490 + tHair; 
			
 			changeHair5 = 30230 + tHair; 
 			changeHair6 = 30260 + tHair; 
 			changeHair7 = 30280 + tHair; 
 			changeHair8 = 30240 + tHair; 
 			changeHair9 = 30290 + tHair; 
 			changeHair10 = 30270 + tHair; 
 			changeHair11 = 30340 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。有看到你所喜歡的造型嗎？只要你有#b#t5150005##k就可以把造型更改成你所喜歡的樣式！", 5150005, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31040 + tHair; 
 			changeHair2 = 31000 + tHair; 
 			changeHair3 = 31670 + tHair; 
			
 			changeHair4 = 31250 + tHair; 
 			changeHair5 = 31220 + tHair; 
 			changeHair6 = 31260 + tHair; 
 			changeHair7 = 31240 + tHair; 
 			changeHair8 = 31110 + tHair; 
 			changeHair9 = 31270 + tHair; 
 			changeHair10 = 31030 + tHair; 
 			changeHair11 = 31230 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。有看到你所喜歡的造型嗎？只要你有#b#t5150005##k就可以把造型更改成你所喜歡的樣式！", 5150005, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		} 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair1 = eHair; 
 		changeHair2 = eHair + 1; 
 		changeHair3 = eHair + 7; 
 		changeHair4 = eHair + 3; 
 		changeHair5 = eHair + 4; 
 		changeHair6 = eHair + 5; 

 		mHair = self->askAvatar( "以下是染髮的種類預覽。有看到你所喜歡的造型嗎？只要你有#b#t5151005##k就可以把造型更改成你所喜歡的樣式！", 5151005, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。對我的技術還滿意吧？如果還有需要，以後請務必回來找我。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso tingir seu cabelo sem ele. Me desculpe." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Por favor, volte depois." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，必須有高級會員卡才可以更換成你喜歡的造型。" ); 
 	} 
 	else if ( selectHair == 2 ) { 
 		cHair = target->getHair(); 
 		if ( cHair == 31240 or cHair == 31241 or cHair == 31242 or cHair == 31243 or cHair == 31244 or cHair == 31245 or cHair == 31246 or cHair == 31247 ) self->say( "因為我只是個學徒，還在學習當中，目前還無法為你的頭髮樣式進行染色。" ); 
 		else { 
 			nRet1 = self->askYesNo( "有攜帶#b#t5154000##k嗎？我可以幫你把頭髮#b隨機進行染色#k！確定要使用#b#t5154000##k進行染髮？" ); 
 			if ( nRet1 == 0 ) self->say( "好的...等你做好決定以後再回來找我吧。" ); 
 			else { 
 				tHair = target->getHair() % 10; 
 				changeHair = 31240 + tHair; 
 				mHair = self->makeRandAvatar( 5154000, changeHair ); 

 				if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 				else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 				else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 				else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 			} 
 		} 
 	} 
--[[
 	else if ( selectHair == 3 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30490 + tHair; 
			
 			changeHair5 = 30230 + tHair; 
 			changeHair6 = 30260 + tHair; 
 			changeHair7 = 30280 + tHair; 
 			changeHair8 = 30240 + tHair; 
 			changeHair9 = 30290 + tHair; 
 			changeHair10 = 30270 + tHair; 
 			changeHair11 = 30340 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。有看到你所喜歡的造型嗎？只要你有#b#t5420004##k eu mudarei isso para voc? Com este cupom, voc?tem o poder de mudar o estilo do seu cabelo para algo novo, sempre que quiser, por UM M邘! Agora, escolha um estilo que goste.", 5420004, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31040 + tHair; 
 			changeHair2 = 31000 + tHair; 
 			changeHair3 = 31670 + tHair; 
			
 			changeHair4 = 31250 + tHair; 
 			changeHair5 = 31220 + tHair; 
 			changeHair6 = 31260 + tHair; 
 			changeHair7 = 31240 + tHair; 
 			changeHair8 = 31110 + tHair; 
 			changeHair9 = 31270 + tHair; 
 			changeHair10 = 31030 + tHair; 
 			changeHair11 = 31230 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。有看到你所喜歡的造型嗎？只要你有#b#t5420004##k eu mudarei isso para voc? Com este cupom, voc?tem o poder de mudar o estilo do seu cabelo para algo novo, sempre que quiser, por UM M邘! Agora, escolha um estilo que goste.", 5420004, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		} 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 ]]--
 } 

-- Orbis EXP hair salon 
function s_hair_orbis2() {
-- 		selectHair = self->askMenu( "你好，我是#p2012007#，目前是這裡的學徒，你有攜帶#b#t5150013##k或#b#t5151004##k嗎？如果有的話我可以為你更換最時尚的造型，請問需要什麼服務呢？\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L2# 更換髮色 (使用一般會員卡)#l" ); 
		selectHair = self->askMenu( "你好，我是#p2012007#，目前是這裡的學徒，你有攜帶#b#t5150004##k或#b#t5151004##k嗎？如果有的話我可以為你更換最時尚的造型，請問需要什麼服務呢？\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L2# 更換髮色 (使用一般會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
-- 		nRet1 = self->askYesNo( "Se usar o cupom EXP, seu cabelo vai mudar aleatoriamente, com a chance de ganhar um novo estilo experimental que nunca viu antes. Ainda quer usar #b#t5150013##k進行染髮？" ); 
		nRet1 = self->askYesNo( "由於我還只是個學徒，使用一般券目前只能進行#b隨機更換#k造型，而且如果後悔，我也沒辦法退還會員卡。你確定要使用#b#t5150004##k進行染髮？" ); 
 		if ( nRet1 == 0 ) self->say( "沒關係...但我想你真的擔心過頭了，我對我自己的實力是非常有自信的！"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 
--[[
 				changeHair1 = 30530 + tHair; 
 				changeHair2 = 30630 + tHair; 
 				changeHair3 = 30760 + tHair; 
				
 				changeHair4 = 30230 + tHair; 
 				changeHair5 = 30260 + tHair; 
 				changeHair6 = 30280 + tHair; 
 				changeHair7 = 30240 + tHair; 
 				changeHair8 = 30290 + tHair; 
 				changeHair9 = 30270 + tHair; 
 				changeHair10 = 30340 + tHair; 
 				changeHair11 = 30370 + tHair; 
				
 				mHair = self->makeRandAvatar( 5150013,changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 ]]--
				changeHair1 = 30000 + tHair;
				changeHair2 = 30020 + tHair;
				changeHair3 = 30030 + tHair;
				changeHair4 = 30230 + tHair;
				changeHair5 = 30240 + tHair;
				changeHair6 = 30260 + tHair;
				changeHair7 = 30270 + tHair;
				changeHair8 = 30280 + tHair;
				changeHair9 = 30290 + tHair;
				changeHair10 = 30340 + tHair;
				changeHair11 = 30610 + tHair;
				changeHair12 = 30440 + tHair;
				changeHair13 = 30400 + tHair;
				
				mHair = self->makeRandAvatar( 5150004, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 );
 			} 
 			else if ( target->getGender() == 1 ) { 
--[[
 				changeHair1 = 31530 + tHair; 
 				changeHair2 = 31630 + tHair; 
 				changeHair3 = 31710 + tHair; 
				
 				changeHair4 = 31250 + tHair; 
 				changeHair5 = 31220 + tHair; 
 				changeHair6 = 31260 + tHair; 
 				changeHair7 = 31240 + tHair; 
 				changeHair8 = 31110 + tHair; 
 				changeHair9 = 31270 + tHair; 
 				changeHair10 = 31030 + tHair; 
 				changeHair11 = 31230 + tHair; 
 				changeHair12 = 31650 + tHair;	--Black Dashing Damsel 
 				changeHair13 = 31320 + tHair; 

 				mHair = self->makeRandAvatar( 5150013, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 ]]--
				changeHair1 = 31000 + tHair;
				changeHair2 = 31030 + tHair;
				changeHair3 = 31040 + tHair;
				changeHair4 = 31110 + tHair;
				changeHair5 = 31220 + tHair;
				changeHair6 = 31230 + tHair;
				changeHair7 = 31240 + tHair;
				changeHair8 = 31250 + tHair;
				changeHair9 = 31260 + tHair;
				changeHair10 = 31270 + tHair;
				changeHair11 = 31320 + tHair;
				changeHair12 = 31700 + tHair;
				changeHair13 = 31620 + tHair;
				changeHair14 = 31610 + tHair;
				
				mHair = self->makeRandAvatar( 5150004, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13, changeHair14 );
 			} 
 			if ( mHair == 1 ) self->say( "完成了，你可以用鏡子看看新的造型...如何？還算滿意吧！如果還有需要的話可以再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo. ?uma pena, mas n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 		} 
 	} 
	
 	else if ( selectHair == 2 ) { 
 		nRet1 = self->askYesNo( "使用一般的會員卡，我只能幫你#b隨機更換髮色#k，你確定要使用#b#t5151004##k來更換造型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "沒關係...但我想你真的擔心過頭了，我對我自己的實力是非常有自信的！"); 
 		else if ( nRet1 == 1 ) { 
 			cHair = target->getHair(); 
 			eHair = cHair - ( cHair % 10 ); 

 			changeHair1 = eHair; 
 			changeHair2 = eHair + 1; 
 			changeHair3 = eHair + 7; 
 			changeHair4 = eHair + 3; 
 			changeHair5 = eHair + 4; 
 			changeHair6 = eHair + 5; 

 			mHair = self->makeRandAvatar( 5151004, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 			if ( mHair == 1 ) self->say( "完成了！你可以到這邊的鏡子看看新的髮型。怎麼樣，還算滿意對吧？哈哈，如果有需要還可以來找我。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso tingir seu cabelo sem ele. Me desculpe." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Por favor, volte depois." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 		} 
 	} 
 } 

-- Ludibrium VIP Hair-Salon    
function s_hair_ludi1() {
 	selectHair = self->askMenu( "歡..迎 歡..迎, 歡..迎來到玩具城美髮店，你有攜帶#b#t5150007##k或#b#t5151007##k嗎？如果你有這些會員卡，那我可以幫你更換一個會讓你非常滿意的造型...\r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮型顏色 (使用高級會員卡)#l\r\n#L2# 更換髮型 (使用VIP會員卡 ※#r台版僅供預覽#b※)#l" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30660 + tHair; 
			
 			changeHair5 = 30250 + tHair; 
 			changeHair6 = 30190 + tHair; 
 			changeHair7 = 30150 + tHair; 
 			changeHair8 = 30050 + tHair; 
 			changeHair9 = 30280 + tHair; 
 			changeHair10 = 30240 + tHair; 
 			changeHair11 = 30300 + tHair; 
 			changeHair12 = 30160 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150007##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150007, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31040 + tHair; 
 			changeHair2 = 31000 + tHair; 
 			changeHair3 = 31550 + tHair; 
			
 			changeHair4 = 31150 + tHair; 
 			changeHair5 = 31280 + tHair; 
 			changeHair6 = 31160 + tHair; 
 			changeHair7 = 31120 + tHair; 
 			changeHair8 = 31290 + tHair; 
 			changeHair9 = 31270 + tHair; 
 			changeHair10 = 31030 + tHair; 
 			changeHair11 = 31230 + tHair; 
 			changeHair12 = 31010 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150007##k，我就可以幫你更換成下列任意一種你喜歡的造型!", 5150007, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 				if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 				else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 				else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 				else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair1 = eHair; 
 		changeHair2 = eHair + 2; 
 		changeHair4 = eHair + 3; 
 		changeHair6 = eHair + 4; 
 		changeHair3 = eHair + 7; 
 		changeHair5 = eHair + 5; 

 		mHair = self->askAvatar( "以下是染髮的種類預覽。只要你有攜帶#b#t5151007##k，我就可以幫你把頭髮染成你喜歡的顏色！", 5151007, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 				if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 				else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 				else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 				else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 	else if ( selectHair == 2 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30030 + tHair; 
 			changeHair2 = 30020 + tHair; 
 			changeHair3 = 30000 + tHair; 
 			changeHair4 = 30660 + tHair; 
			
 			changeHair5 = 30250 + tHair; 
 			changeHair6 = 30190 + tHair; 
 			changeHair7 = 30150 + tHair; 
 			changeHair8 = 30050 + tHair; 
 			changeHair9 = 30280 + tHair; 
 			changeHair10 = 30240 + tHair; 
 			changeHair11 = 30300 + tHair; 
 			changeHair12 = 30160 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420005##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5420005, {changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12} ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31040 + tHair; 
 			changeHair2 = 31000 + tHair; 
 			changeHair3 = 31550 + tHair; 
			
 			changeHair4 = 31150 + tHair; 
 			changeHair5 = 31280 + tHair; 
 			changeHair6 = 31160 + tHair; 
 			changeHair7 = 31120 + tHair; 
 			changeHair8 = 31290 + tHair; 
 			changeHair9 = 31270 + tHair; 
 			changeHair10 = 31030 + tHair; 
 			changeHair11 = 31230 + tHair; 
 			changeHair12 = 31010 + tHair; 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420005##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5420005, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 		} 
 				if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 				else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 				else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 				else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 } 

-- Ludibrium EXP Hair-Salon 
function s_hair_ludi2() {
 		selectHair = self->askMenu( "歡迎來到玩具城美髮店，只要你有攜帶#b#t5150006##k或者#b#t5151006##k我就可以幫你更換造型。但是因為我目前還只是個學徒，你的造型將#b隨機決定#k，但不必太過擔心，我相信你會非常滿意的。你需要哪一種服務呢？\r\n#b#L0# 使用#t5150006#更換髮型#l\r\n#L2# 使用#t5151006#更換髮型顏色#l" ); 
 	if ( selectHair == 0 ) { 
 		nRet1 = self->askYesNo( "由於我還只是個學徒，使用一般會員券，只能#b隨機更換#k造型。確定要使用#b#t5151006##k來更換造型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "不必太過擔心，儘管我只是個學徒，也能把你變得非常時尚的。"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 
 				changeHair1 = 30540 + tHair; 
 				changeHair2 = 30640 + tHair; 
 				changeHair3 = 30680 + tHair; 
				
 				changeHair4 = 30250 + tHair; 
 				changeHair5 = 30190 + tHair; 
 				changeHair6 = 30150 + tHair; 
 				changeHair7 = 30050 + tHair; 
 				changeHair8 = 30280 + tHair; 
 				changeHair9 = 30240 + tHair; 
 				changeHair10 = 30300 + tHair; 
 				changeHair11 = 30160 + tHair; 
 				changeHair12 = 30650 + tHair; 
				
 				mHair = self->makeRandAvatar( 5150006, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12 ); 
 			} 
 			else if ( target->getGender() == 1 ) { 
 				changeHair1 = 31540 + tHair; 
 				changeHair2 = 31640 + tHair; 
 				changeHair3 = 31600 + tHair;	--Tall Bun  
				
 				changeHair4 = 31150 + tHair; 
 				changeHair5 = 31280 + tHair; 
 				changeHair6 = 31160 + tHair; 
 				changeHair7 = 31120 + tHair; 
 				changeHair8 = 31290 + tHair; 
 				changeHair9 = 31270 + tHair; 
 				changeHair10 = 31030 + tHair; 
 				changeHair11 = 31230 + tHair; 
 				changeHair12 = 31010 + tHair; 
 				changeHair13 = 31680 + tHair;	--Black Lovely Ladyhawk 
				
 				mHair = self->makeRandAvatar( 5150006, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 			} 
 			if ( mHair == 1 ) self->say( "完成了，你可以用鏡子看看新的造型...如何？還算滿意吧！如果還有需要的話可以再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo. ?uma pena, mas n緌 posso cortar seu cabelo sem ele. Me desculpe." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Por favor, volte depois." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 		} 
 	} 
	
 	else if ( selectHair == 2 ) { 
 		nRet1 = self->askYesNo( "由於我還只是個學徒，使用一般券，只能#b隨機更換#k髮色。確定要使用#b#t5151006##k來更換造型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "不必太過擔心，儘管我只是個學徒，也能把你變得非常時尚的。"); 
 		else if ( nRet1 == 1 ) { 
 			cHair = target->getHair(); 
 			eHair = cHair - ( cHair % 10 ); 

 			changeHair1 = eHair; 
 			changeHair2 = eHair + 2; 
 			changeHair4 = eHair + 3; 
 			changeHair6 = eHair + 4; 
 			changeHair3 = eHair + 7; 
 			changeHair5 = eHair + 5; 

 			mHair = self->makeRandAvatar( 5151006, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 			if ( mHair == 1 ) self->say( "完成了！你可以到這邊的鏡子看看新的髮型。怎麼樣，還算滿意對吧？哈哈，如果有需要還可以來找我。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso tingir seu cabelo sem ele. Me desculpe." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Um pequeno problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Por favor, volte depois." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 		} 
 	} 
 } 

-- ****** Wedding: Hair Salon - Julius Styleman - VIP Style		15 
function s_hair_wedding1() {

 	selectHair = self->askMenu( "歡迎光臨！想要更換造型嗎？只要你有攜帶#b#t5150020##k或者#b#t5151017##k我就可以幫你整形成你所喜歡的樣子，如何，需要什麼服務嗎？\r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮色 (使用高級會員卡)#l\r\n#L2# 更換髮型 (使用VIP會員卡 ※#r台版僅供預覽#b※)#l" ); 

 	if ( selectHair == 0 ) {				-- select change hair style 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) {		-- man's  

 			changeHair1 = 30580 + tHair;	 
 			changeHair2 = 30590 + tHair;	 
 			changeHair3 = 30280 + tHair;	 
 			changeHair4 = 30670 + tHair; 
			
 			changeHair5 = 30410 + tHair;	 
 			changeHair6 = 30200 + tHair;	 
 			changeHair7 = 30050 + tHair;	 
 			changeHair8 = 30230 + tHair;	 
 			changeHair9 = 30290 + tHair;	 
 			changeHair10 = 30300 + tHair;	 
 			changeHair11 = 30250 + tHair;	 
		
 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150020##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150020, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		} 
 		else if ( target->getGender() == 1 ) {	-- woman's 
			
 			changeHair1 = 31580 + tHair;	 
 			changeHair2 = 31590 + tHair;	 
 			changeHair3 = 31310 + tHair;	 
 			changeHair4 = 31200 + tHair;	 

 			changeHair5 = 31150 + tHair;	 
 			changeHair6 = 31160 + tHair;	 
 			changeHair7 = 31020 + tHair;	 
 			changeHair8 = 31260 + tHair;	 
 			changeHair9 = 31230 + tHair;	 
 			changeHair10 = 31220 + tHair;	 
 			changeHair11 = 31110 + tHair;	 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5150020##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5150020, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		}  
 			-- answer about selecting change hair style 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo. N緌 posso cortar seu cabelo sem ele. Desculpe, colega." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 	else if ( selectHair == 1 ) {	-- select change hair color without the sex 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair1 = eHair;		-- Black 
 		changeHair2 = eHair + 3;	-- Blonde 
 		changeHair3 = eHair + 5;	-- Blue 
 		changeHair4 = eHair + 7;	-- Brown 
 		changeHair5 = eHair + 4;	-- Green 
 		changeHair6 = eHair + 1;	-- Red 
	
 		mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5151017##k，我就可以幫你更換成下列任意一種你喜歡的造型...", 5151017, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 		-- answer about selecting change hair color 
 		if ( mHair == 1 ) self->say( "怎麼樣？是不是非常棒啊？如果你還需要更換造型，隨時都可以回到這裡來。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo... Pena, n緌 posso tingir seu cabelo sem ele. Desculpe, colega." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Volte mais tarde." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 

 	else if ( selectHair == 2 ) {				-- select change hair style 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) {		-- man's  

 			changeHair1 = 30580 + tHair;	 
 			changeHair2 = 30590 + tHair;	 
 			changeHair3 = 30280 + tHair;	 
 			changeHair4 = 30670 + tHair; 
			
 			changeHair5 = 30410 + tHair;	 
 			changeHair6 = 30200 + tHair;	 
 			changeHair7 = 30050 + tHair;	 
 			changeHair8 = 30230 + tHair;	 
 			changeHair9 = 30290 + tHair;	 
 			changeHair10 = 30300 + tHair;	 
 			changeHair11 = 30250 + tHair;	 
		
 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420000##k我就可以幫你把髮型改造成最新的樣式，而且保證一定會讓你非常滿意的！", 5420000, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		} 
 		else if ( target->getGender() == 1 ) {	-- woman's 
			
 			changeHair1 = 31580 + tHair;	 
 			changeHair2 = 31590 + tHair;	 
 			changeHair3 = 31310 + tHair;	 
 			changeHair4 = 31200 + tHair;	 

 			changeHair5 = 31150 + tHair;	 
 			changeHair6 = 31160 + tHair;	 
 			changeHair7 = 31020 + tHair;	 
 			changeHair8 = 31260 + tHair;	 
 			changeHair9 = 31230 + tHair;	 
 			changeHair10 = 31220 + tHair;	 
 			changeHair11 = 31110 + tHair;	 

 			mHair = self->askAvatar( "這裡是最新造型的預覽。只要你有攜帶#b#t5420000##k我就可以幫你把髮型改造成最新的樣式，而且保證一定會讓你非常滿意的！", 5420000, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 		}  
 			-- answer about selecting change hair style 
 		if ( mHair == 1 ) self->say( "如何啊？這個造型非常不錯對吧？哈哈。如果以後還需要一個新的造型，請來找我，我保證會讓你非常滿意的。" ); 
 		else if ( mHair == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom certo. N緌 posso cortar seu cabelo sem ele. Desculpe, colega." ); 
 		else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 	} 
 } 

-- ****** Wedding : Hair Salon - Salon Seamus - EXP Style		16 
function s_hair_wedding2() {

 		selectHair = self->askMenu( "嗨，對現在的造型覺得厭倦了嗎？不妨嘗試新的造型看看吧！只要你有帶#b#t5150019##k或#b#t5151016##k我就可以幫你變換造型，怎麼樣呢？...#b\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L1# 更換髮色 (使用一般會員卡)#l" ); 

 		if ( selectHair == 0 ) {		-- style 
 			nRet1 = self->askYesNo( "使用一般的會員卡，我只能提供#b隨機造型更換#k的服務，因此就算你對新的造型不滿意，我也無法退還任何費用，如何，確定要使用#b#t5150019##k來更換造型嗎？" ); 

 			if ( nRet1 == 0 ) self->say( "沒問題...等你有需要的時候可以回來找我。"); 
			
 			else if ( nRet1 == 1 ) { 
 				tHair = target->getHair() % 10; 

 				if ( target->getGender() == 0 ) { 
 					changeHair1 = 30570 + tHair; 
 					changeHair2 = 30450 + tHair;					 
		
 					changeHair4 = 30410 + tHair;	 
 					changeHair5 = 30200 + tHair;	 
 					changeHair6 = 30050 + tHair;	 
 					changeHair7 = 30230 + tHair;	 
 					changeHair8 = 30290 + tHair;	 
 					changeHair9 = 30300 + tHair;	 
 					changeHair10 = 30250 + tHair;	 
 					changeHair11 = 30690 + tHair;	 
					
 					mHair = self->makeRandAvatar( 5150019, changeHair1, changeHair2, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 				} 
 				else if ( target->getGender() == 1 ) { 
 					changeHair1 = 31570 + tHair; 
 					changeHair2 = 31480 + tHair;						 

 					changeHair4 = 31150 + tHair;	 
 					changeHair5 = 31160 + tHair;	 
 					changeHair6 = 31020 + tHair;	 
 					changeHair7 = 31260 + tHair;	 
 					changeHair8 = 31230 + tHair;	 
 					changeHair9 = 31220 + tHair;	 
 					changeHair10 = 31110 + tHair; 
 					changeHair11 = 31490 + tHair;					 
							
 					mHair = self->makeRandAvatar( 5150019, changeHair1, changeHair2,changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 				} 

 				if ( mHair == 1 ) self->say( "怎麼樣？這個造型非常棒對吧！現在你也是時尚玩家了。如果還想更換造型，歡迎再度光臨。" ); 
 				else if ( mHair == -1 ) self->say( "Hum... Tem certeza de que tem o cupom certo? Desculpe, mas nada de corte de cabelo sem ele." ); 
 				else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 				else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" ); 
 			} 
 		} 
	
 		else if ( selectHair == 1 ) {		-- color 

 			nRet1 = self->askYesNo( "如果你使用#b#t5151016##k你的髮型顏色會#b隨機改變#k。你仍然想要使用一般會員卡進行染色嗎？" ); 
 			if ( nRet1 == 0 ) self->say( "沒問題...等你有需要的時候可以回來找我。"); 
 			else if ( nRet1 == 1 ) { 
 				cHair = target->getHair(); 
 				eHair = cHair - ( cHair % 10 ); 

 				changeHair1 = eHair;		-- Black 
 				changeHair2 = eHair + 3;	-- Blonde 
 				changeHair3 = eHair + 5;	-- Blue 
 				changeHair4 = eHair + 7;	-- Brown 
 				changeHair5 = eHair + 4;	-- Green 
 				changeHair6 = eHair + 1;	-- Red 

 				if ( target->getGender() == 0 ) mHair = self->makeRandAvatar( 5151016, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 
 				else if ( target->getGender() == 1 ) mHair = self->makeRandAvatar( 5151016, changeHair1, changeHair2, changeHair3, changeHair4, changeHair6 ); 

 			if ( mHair == 1 ) self->say( "怎麼樣？這個造型非常棒對吧！現在你也是時尚玩家了。如果還想更換造型，歡迎再度光臨。" ); 
 			else if ( mHair == -1 ) self->say( "Hum... Tem certeza de que tem o cupom certo? Desculpe, mas nada de corte de cabelo sem ele." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa tingir seu cabelo neste momento. Volte mais tarde." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你必須攜帶會員卡我才能幫你更換造型。" ); 
 			} 
 		} 
	
 } 


-- ****** Wedding : Hair Salon - Claudia - EXP Style		16 
function s_hair_wedding3() {
	
 	qr = target.questRecord; 
 	val = qr.get( 8861 ); 
	
 	if (val == "end" )  
 		self->say( "Mudei seu cabelo uma vez em troca de servi蔞s, esporte. Voc?deve obter um cupom Experimental na Loja de Itens se quiser troc?lo novamente."); 

 	else { 
 		nRet1 = self->askYesNo( "Est?#Gpronto:pronta# para fazermos um penteado incr癉el? Eu acho que voc?est? Somente diga a palavra certa e come蓷remos!" ); 

 		if ( nRet1 == 0 ) self->say( "T?bem, lhe darei um minuto."); 
		
 		else if ( nRet1 == 1 ) { 
 			self->say( "L?vamos n鏀!"); 
			
 			--tHair = target->getHair() % 10; 

 			if ( target->getGender() == 0 ) { 
 				changeHair1 = 30032;	-- Orange Buzz 
 				changeHair4 = 30020;	-- Black Rebel 
 				changeHair5 = 30000;	-- Black Toben 
 				changeHair6 = 30132;	-- Orange Antagonist 
 				changeHair7 = 30192;	-- Orange Bowl Cut 
 				changeHair8 = 30240;	-- Black Monkey 
 				changeHair9 = 30162;	-- Orange Trip Scratch 
 				changeHair10 = 30270;	-- Black w/ Bald Spot 
 				changeHair11 = 30112;	-- Orange Fireball 

 				mHair = self->makeRandAvatarNormal( 4031528, changeHair1, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 			} 
			
 			else if ( target->getGender() == 1 ) { 
 				changeHair1 = 31050;	-- Black Connie 
 				changeHair4 = 31040;	-- Black Edgy 
 				changeHair5 = 31030;	-- Black polly 
 				changeHair6 = 31001;	-- Red Sammy 
 				changeHair7 = 31070;	-- Black Stella 
 				changeHair8 = 31310;	-- Black Carla 
 				changeHair9 = 31091;	-- Red Bridget 
 				changeHair10 = 31250;	-- Black Bowlcut 
 				changeHair11 = 31150;	-- Black Angelica 

 				mHair = self->makeRandAvatarNormal( 4031528, changeHair1,changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11 ); 
 			} 

 			if ( mHair == 1 ) { 
 				self->say( "Nada mau, eu diria... Sabia que os livros que estudei viriam a calhar..." ); 
 				qr.set( 8861, "end" );	 
 			} 
 			else if ( mHair == -1 ) self->say( "Hum... Tem certeza de que tem o cupom gr嫢is certo? Desculpe, mas nada de corte de cabelo sem ele." ); 
 			else if ( mHair == -3 ) self->say( "Me desculpe. Parece que temos um problema aqui no sal緌. N緌 acho que possa cortar seu cabelo neste momento. Volte mais tarde." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你的身上似乎沒有攜帶會員券，因此我無法幫你改變造型。" );				 
 			} 
 	} 

 } 

-- NLC VIP hair salon 
function s_NLC_HairVip() {
 	selectHair = self->askMenu( "嗨，你好，我是這裡的首席美髮師，只要你有#b#t5150031##k、#b#t5151026##k或#b#t5420001##k我就可以幫你把造型變更成你所喜歡的任意造型。\r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮色 (使用高級會員卡)#l\r\n#L2# 更換髮型 (使用VIP會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
						
 			changeHair2 = 30730 + tHair; 
 			changeHair3 = 30280 + tHair; 
 			changeHair4 = 30220 + tHair; 
			
 			changeHair5 = 30410 + tHair; 
 			changeHair6 = 30200 + tHair; 
 			changeHair7 = 30050 + tHair; 
 			changeHair8 = 30230 + tHair; 
 			changeHair9 = 30160 + tHair; 
 			changeHair10 = 30110 + tHair; 
 			changeHair11 = 30250 + tHair;	 

 			mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5150031##k來更換造型嗎？", 5150031, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 		} 
 		else if ( target->getGender() == 1 ) { 
					
 			changeHair2 = 31730 + tHair; 
 			changeHair3 = 31310 + tHair; 
 			changeHair4 = 31470 + tHair; 
			
 			changeHair5 = 31150 + tHair; 
 			changeHair6 = 31160 + tHair; 
 			changeHair7 = 31300 + tHair; 
 			changeHair8 = 31260 + tHair; 
 			changeHair9 = 31220 + tHair; 
 			changeHair10 = 31410 + tHair; 
 			changeHair11 = 31270 + tHair;			 

 			mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5150031##k來更換造型嗎？", 5150031, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 		} 
 		if ( mHair == 1 ) self->say( "非常棒！你覺得呢？簡直就是藝術！哈哈哈哈。如果你想在更換造型，請再跟我說！" ); 
 		else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 		else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later" ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair0 = eHair; 
 		changeHair1 = eHair + 1; 
 		changeHair2 = eHair + 2; 
 		changeHair3 = eHair + 3; 
 		changeHair4 = eHair + 4; 
 		changeHair5 = eHair + 5; 
 		changeHair6 = eHair + 6; 
 		changeHair7 = eHair + 7; 

 		mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5151026##k來更換造型嗎？", 5151026, changeHair0,changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ,changeHair7 ); 

 		if ( mHair == 1 ) self->say( "非常棒！你覺得呢？簡直就是藝術！哈哈哈哈。如果你想在更換造型，請再跟我說！" ); 
 		else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't dye your hair without it. I'm sorry." ); 
 		else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to dye your hair at this very moment. Please come back later." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 	else if ( selectHair == 2 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
						
 			changeHair2 = 30730 + tHair; 
 			changeHair3 = 30280 + tHair; 
 			changeHair4 = 30220 + tHair; 
			
 			changeHair5 = 30410 + tHair; 
 			changeHair6 = 30200 + tHair; 
 			changeHair7 = 30050 + tHair; 
 			changeHair8 = 30230 + tHair; 
 			changeHair9 = 30160 + tHair; 
 			changeHair10 = 30110 + tHair; 
 			changeHair11 = 30250 + tHair;	 

 			mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5420001##k來更換造型嗎？", 5420001, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 		} 
 		else if ( target->getGender() == 1 ) { 
					
 			changeHair2 = 31730 + tHair; 
 			changeHair3 = 31310 + tHair; 
 			changeHair4 = 31470 + tHair; 
			
 			changeHair5 = 31150 + tHair; 
 			changeHair6 = 31160 + tHair; 
 			changeHair7 = 31300 + tHair; 
 			changeHair8 = 31260 + tHair; 
 			changeHair9 = 31220 + tHair; 
 			changeHair10 = 31410 + tHair; 
 			changeHair11 = 31270 + tHair;			 

 			mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5420001##k來更換造型嗎？", 5420001, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 		} 
 		if ( mHair == 1 ) self->say( "非常棒！你覺得呢？簡直就是藝術！哈哈哈哈。如果你想在更換造型，請再跟我說！" ); 
 		else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 		else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later" ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 } 

-- NLC EXP. hair salon 
function s_NLC_HairExp() {
 	selectHair = self->askMenu( "你好，我是本店的助理，只要你有#b#t5150030##k或#b#t5151025##k，那麼我就可以為你提供造型服務。怎麼樣呢？需要什麼服務嗎？\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L1# 更換髮色 (使用一般會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		nRet1 = self->askYesNo( "如果你使用一般會員卡，你的造型將會#b隨機#k決定，你確定要使用#b#t5150030##k來更換造型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "我知道了...你可以再考慮一下，如果決定好了，可以再回來找我。"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 

 				changeHair1 = 30400 + tHair; 
 				changeHair2 = 30360 + tHair; 
 				changeHair3 = 30440 + tHair; 
				
 				changeHair5 = 30410 + tHair; 
 				changeHair6 = 30200 + tHair; 
 				changeHair7 = 30050 + tHair; 
 				changeHair8 = 30230 + tHair; 
 				changeHair9 = 30160 + tHair; 
 				changeHair10 = 30110 + tHair; 
 				changeHair11 = 30250 + tHair;				 
				
 				mHair = self->makeRandAvatar( 5150030, changeHair1, changeHair2, changeHair3, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 			} 
 			else if ( target->getGender() == 1 ) { 
 				changeHair1 = 31560 + tHair; 
 				changeHair2 = 31720 + tHair; 
 				changeHair3 = 31450 + tHair; 
			
 				changeHair5 = 31150 + tHair; 
 				changeHair6 = 31160 + tHair; 
 				changeHair7 = 31300 + tHair; 
 				changeHair8 = 31260 + tHair; 
 				changeHair9 = 31220 + tHair; 
 				changeHair10 = 31410 + tHair; 
 				changeHair11 = 31270 + tHair;				 

 				mHair = self->makeRandAvatar( 5150030, changeHair1, changeHair2, changeHair3, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11); 
 			} 
 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 		} 
 	} 

 	else if ( selectHair == 1 ) { 
 		nRet1 = self->askYesNo( "如果你使用一般會員卡，你的造型將會#b隨機#k決定，你確定要使用#b#t5151025##k來更換造型？" ); 
 		if ( nRet1 == 0 ) self->say( "我知道了...你可以再考慮一下，如果決定好了，可以再回來找我。"); 
 		else if ( nRet1 == 1 ) { 
 			cHair = target->getHair(); 
 			eHair = cHair - ( cHair % 10 ); 

 			changeHair0 = eHair; 
 			changeHair1 = eHair + 1; 
 			changeHair2 = eHair + 2; 
 			changeHair3 = eHair + 3; 
 			changeHair4 = eHair + 4; 
 			changeHair5 = eHair + 5; 
 			changeHair6 = eHair + 6; 
 			changeHair7 = eHair + 7; 

 			mHair = self->makeRandAvatar( 5151025, changeHair0,changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7 ); 

 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't dye your hair without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to dye your hair at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 		} 
 	} 
 } 

-- mureung VIP Hairshop 
function s_hair_mureung1() {
 	selectHair = self->askMenu( "歡迎來到武陵美髮店。如果你身上有帶#b#t5150025##k或者#b#t5151020##k，我就可以幫你把造型變更成你所喜歡的任意造型。\r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮色 (使用高級會員卡)#l\r\n#L2# 更換髮型 (使用VIP會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30600 + tHair; 
 			changeHair2 = 30750 + tHair; 
			
 			changeHair3 = 30250 + tHair; 
 			changeHair4 = 30150 + tHair; 
 			changeHair5 = 30240 + tHair; 
 			changeHair6 = 30350 + tHair; 
 			changeHair7 = 30180 + tHair; 
 			changeHair8 = 30300 + tHair; 
 			changeHair9 = 30270 + tHair; 
 			changeHair10 = 30160 + tHair; 
			
 			mHair = self->askAvatar( "以下是本店提供的造型樣本，只要你有#b#t5150025##k，我就可以幫你更改成你所喜歡的任意造型！", 5150025, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31460 + tHair; 
 			changeHair2 = 31040 + tHair; 
 			changeHair3 = 31180 + tHair; 
			
 			changeHair4 = 31310 + tHair; 
 			changeHair5 = 31300 + tHair; 
 			changeHair6 = 31160 + tHair; 
 			changeHair7 = 31030 + tHair; 
 			changeHair8 = 31250 + tHair; 
 			changeHair9 = 31220 + tHair; 
 			changeHair10 = 31230 + tHair; 

 			mHair = self->askAvatar( "以下是本店提供的造型樣本，只要你有#b#t5150025##k，我就可以幫你更改成你所喜歡的任意造型！", 5150025, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10 ); 
 		} 
 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		changeHair1 = eHair; 
 		changeHair2 = eHair + 1; 
 		changeHair4 = eHair + 3; 
 		changeHair6 = eHair + 6; 
 		changeHair3 = eHair + 7; 
 		changeHair5 = eHair + 5; 

 		mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5151020##k來更改成你所喜歡的造型嗎？", 5151020, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't dye your hair without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to dye your hair at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 	else if ( selectHair == 2 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30600 + tHair; 
 			changeHair2 = 30750 + tHair; 
			
 			changeHair3 = 30250 + tHair; 
 			changeHair4 = 30150 + tHair; 
 			changeHair5 = 30240 + tHair; 
 			changeHair6 = 30350 + tHair; 
 			changeHair7 = 30180 + tHair; 
 			changeHair8 = 30300 + tHair; 
 			changeHair9 = 30270 + tHair; 
 			changeHair10 = 30160 + tHair; 
			
 			mHair = self->askAvatar( "以下是本店提供的造型樣本，只要你有#b#t5150025##k，我就可以幫你更改成你所喜歡的任意造型！", 5420006, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31460 + tHair; 
 			changeHair2 = 31040 + tHair; 
 			changeHair3 = 31180 + tHair; 
			
 			changeHair4 = 31310 + tHair; 
 			changeHair5 = 31300 + tHair; 
 			changeHair6 = 31160 + tHair; 
 			changeHair7 = 31030 + tHair; 
 			changeHair8 = 31250 + tHair; 
 			changeHair9 = 31220 + tHair; 
 			changeHair10 = 31230 + tHair; 

 			mHair = self->askAvatar( "以下是本店提供的造型樣本，只要你有#b#t5150025##k，我就可以幫你更改成你所喜歡的任意造型！", 5420006, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10 ); 
 		} 
 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 } 

-- mureung EXP Hairshop 
function s_hair_mureung2() {
 	selectHair = self->askMenu( "你好，我是本店的助理，只要你有#b#t5150024##k或者#b#t5151019##k，那麼我就可以為你提供造型更改服務。如何呢？\r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L1# 更換髮色 (使用一般會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		nRet1 = self->askYesNo( "如果你使用一般會員卡，你的造型將會#b隨機#k決定，你確定要使用#b#t5150024##k來更換造型？" ); 
 		if ( nRet1 == 0 ) self->say( "我知道了...你可以再考慮一下，如果決定好了，可以再回來找我。"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 
 				changeHair1 = 30700 + tHair; 
 				changeHair2 = 30720 + tHair; 
 				changeHair3 = 30420 + tHair; 
				
 				changeHair4 = 30250 + tHair; 
 				changeHair5 = 30150 + tHair; 
 				changeHair6 = 30240 + tHair; 
 				changeHair7 = 30350 + tHair; 
 				changeHair8 = 30180 + tHair; 
 				changeHair9 = 30300 + tHair; 
 				changeHair10 = 30270 + tHair; 
 				changeHair11 = 30160 + tHair; 

 				mHair = self->makeRandAvatar( 5150024, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10,changeHair11 ); 
 			} 
 			else if ( target->getGender() == 1 ) { 
 				changeHair1 = 31690 + tHair; 
 				changeHair2 = 31210 + tHair; 
 				changeHair3 = 31170 + tHair; 
				
 				changeHair4 = 31310 + tHair; 
 				changeHair5 = 31300 + tHair; 
 				changeHair6 = 31160 + tHair; 
 				changeHair7 = 31030 + tHair; 
 				changeHair8 = 31250 + tHair; 
 				changeHair9 = 31220 + tHair; 
 				changeHair10 = 31230 + tHair; 

 				mHair = self->makeRandAvatar( 5150024, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10 ); 
 			} 
 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 		} 
 	} 
 	else if ( selectHair == 1 ) { 
 		nRet1 = self->askYesNo( "如果你使用一般會員卡，你的造型將會#b隨機#k決定，你確定要使用#b#t5151019##k來更換造型？" ); 
 		if ( nRet1 == 0 ) self->say( "我知道了...你可以再考慮一下，如果決定好了，可以再回來找我。"); 
 		else if ( nRet1 == 1 ) { 
 			cHair = target->getHair(); 
 			eHair = cHair - ( cHair % 10 ); 

 			changeHair1 = eHair; 
 			changeHair2 = eHair + 1; 
 			changeHair4 = eHair + 3; 
 			changeHair6 = eHair + 6; 
 			changeHair3 = eHair + 7; 
 			changeHair5 = eHair + 5; 

 			mHair = self->makeRandAvatar( 5151019, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6 ); 

 			if ( mHair == 1 ) self->say( "你可以用鏡子看看你的新造型。如何呢？雖然不是非常完美，但還算不錯吧？如果你還有需要，歡迎再來找我！" ); 
 			else if ( mHair == -1 ) self->say( "Hmmm...it looks like you don't have our designated coupon...I'm afraid I can't dye your hair without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. A slight problem here at the hair salon. I don't think I'll be able to dye your hair at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 		} 
 	} 
 } 

-- 澗諦 堅晝 ?儩??? 
function s_hair_shouwa1() {
 	selectHair = self->askMenu( "歡迎光臨，我是本店的店長，只要你有攜帶#b#t5150009##k或者#b#t5151009##k，何不讓我來替你更換成最新時尚的造型呢？ \r\n#b#L0# 更換髮型 (使用高級會員卡)#l\r\n#L1# 更換髮色 (使用高級會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		tHair = target->getHair() % 10; 
 		if ( target->getGender() == 0 ) { 
 			changeHair1 = 30000 + tHair;  
 			changeHair2 = 30120 + tHair; 
 			changeHair3 = 30140 + tHair; 
 			changeHair4 = 30190 + tHair; 
 			changeHair5 = 30210 + tHair; 
 			changeHair6 = 30270 + tHair; 
 			changeHair7 = 30290 + tHair; 
 			changeHair8 = 30360 + tHair; 
 			changeHair9 = 30220 + tHair; 
 			changeHair10 = 30370 + tHair; 
 			changeHair11 = 30400 + tHair; 
 			changeHair12 = 30440 + tHair; 
 			changeHair13 = 30510 + tHair; 


 			mHair = self->askAvatar( "以下是本店提供的最新時尚造型樣本，只要你有#b#t5150009##k，我就可以幫你更改成你所喜歡的任意造型！ ", 5150009, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 		} 
 		else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31030 + tHair; 
 			changeHair2 = 31050 + tHair; 
 			changeHair3 = 31070 + tHair; 
 			changeHair4 = 31100 + tHair; 
 			changeHair5 = 31120 + tHair; 
 			changeHair6 = 31130 + tHair; 
 			changeHair7 = 31250 + tHair; 
 			changeHair8 = 31340 + tHair; 
 			changeHair9 = 31210 + tHair; 
 			changeHair10 = 31350 + tHair; 
 			changeHair11= 31400 + tHair; 
 			changeHair12= 31440 + tHair; 
 			changeHair13= 31520 + tHair; 

 			mHair = self->askAvatar( "以下是本店提供的最新時尚造型樣本，只要你有#b#t5150009##k，我就可以幫你更改成你所喜歡的任意造型！ ", 5150009, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 		} 
 		if ( mHair == 1 ) self->say( "完成了！用鏡子看看新的造型吧！你覺得了？這個我真的非常拿手。如果你對現在的造型不滿意，可以再回來找我！" ); 
 		else if ( mHair == -1 ) self->say( "Eh? I don't think you have our designated hair salon coupon. I can't give you the haircut without it. " ); 
 		else if ( mHair == -3 ) self->say( "I'm sorry. It looks like we have a problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Come back later." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 	else if ( selectHair == 1 ) { 
 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		if( eHair==30430 or eHair==31430) { 
			 self->say("你目前的髮型無法更換顏色。"); 
			 return;
 		} 
 		else{ 
 		changeHair1 = eHair; 
 		changeHair2 = eHair + 2; 
 		changeHair4 = eHair + 3; 
 		changeHair3 = eHair + 7; 
 		changeHair5 = eHair + 5; 

 		mHair = self->askAvatar( "我可以徹底的改造你的造型，而且你也可以從下面選擇任何一個你所喜歡的造型，如何，要使用#b#t5151009##k來更改成你所喜歡的造型嗎？", 5151009, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5 ); 

 		if ( mHair == 1 ) self->say( "完成了！用鏡子看看新的造型吧！你覺得了？這個我真的非常拿手。如果你對現在的造型不滿意，可以再回來找我！" ); 
 		else if ( mHair == -1 ) self->say( "Eh? I don't think you have our designated hair salon coupon. I can't dye your hair without it." ); 
 		else if ( mHair == -3 ) self->say( "I'm sorry. It looks like we have a problem here at the hair salon. I don't think I'll be able to dye your hair at this very moment. Come back later." ); 
 		else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 	} 
 	} 
 } 

-- 澗諦 橾奩 ?儩??? 
function s_hair_shouwa2() {
 	selectHair = self->askMenu( "你好，我是這裡的助理，如果你有#b#t5150008##k或者#b#t5151008##k，請讓我來幫你做造型服務... \r\n#b#L0# 更換髮型 (使用一般會員卡)#l\r\n#L1# 更換髮色 (使用一般會員卡)#l" ); 
 	if ( selectHair == 0 ) { 
 		nRet1 = self->askYesNo( "如果你使用一般會員卡，你的造型將會#b隨機#k決定，你確定要使用#b#t5150008##k來更換你的髮型嗎？" ); 
 		if ( nRet1 == 0 ) self->say( "我知道了...等你考慮好了以後再說吧。"); 
 		else if ( nRet1 == 1 ) { 
 			tHair = target->getHair() % 10; 
 			if ( target->getGender() == 0 ) { 
 			changeHair1 = 30000 + tHair;  
 			changeHair2 = 30120 + tHair; 
 			changeHair3 = 30140 + tHair; 
 			changeHair4 = 30190 + tHair; 
 			changeHair5 = 30210 + tHair; 
 			changeHair6 = 30270 + tHair; 
 			changeHair7 = 30290 + tHair; 
 			changeHair8 = 30360 + tHair; 
 			changeHair9 = 30220 + tHair; 
 			changeHair10 = 30370 + tHair; 
 			changeHair11 = 30400 + tHair; 
 			changeHair12 = 30440 + tHair; 
 			changeHair13 = 30510 + tHair; 


 				mHair = self->makeRandAvatar( 5150008, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair12 ); 
 			} 
 			else if ( target->getGender() == 1 ) { 
 			changeHair1 = 31030 + tHair; 
 			changeHair2 = 31050 + tHair; 
 			changeHair3 = 31070 + tHair; 
 			changeHair4 = 31100 + tHair; 
 			changeHair5 = 31120 + tHair; 
 			changeHair6 = 31130 + tHair; 
 			changeHair7 = 31250 + tHair; 
 			changeHair8 = 31340 + tHair; 
 			changeHair9 = 31210 + tHair; 
 			changeHair10 = 31350 + tHair; 
 			changeHair11= 31400 + tHair; 
 			changeHair12= 31440 + tHair; 
 			changeHair13= 31520 + tHair; 

 				mHair = self->makeRandAvatar( 5150008, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5, changeHair6, changeHair7, changeHair8, changeHair9, changeHair10, changeHair11, changeHair12, changeHair13 ); 
 			} 
 			if ( mHair == 1 ) self->say( "好了，你可以用鏡子看看新的造型，如何？不像是一個助理做得到的吧？哈哈哈，如果還有需要請回來找我。" ); 
 			else if ( mHair == -1 ) self->say( "I'm really sorry, but it looks like you don't have our designated coupon, and I'm afraid I can't give you a haircut without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. We've encountered a slight problem here at the hair salon. I don't think I'll be able to give you a haircut at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 		} 
 	} 
 	else if ( selectHair == 1 ) { 

 		cHair = target->getHair(); 
 		eHair = cHair - ( cHair % 10 ); 

 		if( eHair==30430 or eHair==31430) { 
			 self->say("你目前的髮型無法更換顏色。"); 
			 return;
 		} 
 		else{ 
 		nRet1 = self->askYesNo( "如果你使用一般會員卡，你的造型將會#b隨機#k決定，你確定要使用#b#t5151008##k來進行染髮？" ); 
 		if ( nRet1 == 0 ) self->say( "我知道了...等你考慮好了以後再說吧。"); 
 		else if ( nRet1 == 1 ) { 
			

 			changeHair1 = eHair; 
 			changeHair2 = eHair + 2; 
 			changeHair4 = eHair + 3; 
 			changeHair3 = eHair + 7; 
 			changeHair5 = eHair + 5; 

 			if ( target->getGender() == 0 ) mHair = self->makeRandAvatar( 5151008, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5 ); 
 			else if ( target->getGender() == 1 ) mHair = self->makeRandAvatar( 5151008, changeHair1, changeHair2, changeHair3, changeHair4, changeHair5 ); 

 			if ( mHair == 1 ) self->say( "好了，你可以用鏡子看看新的造型，如何？不像是一個助理做得到的吧？哈哈哈，如果還有需要請回來找我。" ); 
 			else if ( mHair == -1 ) self->say( "I'm really sorry, but it looks like you don't have our designated coupon, and I'm afraid I can't dye your hair without it. I'm sorry." ); 
 			else if ( mHair == -3 ) self->say( "I'm sorry. We've encountered a slight problem here at the hair salon. I don't think I'll be able to dye your hair at this very moment. Please come back later." ); 
 			else if ( mHair == 0 or mHair == -2 ) self->say( "非常抱歉，你必須攜帶會員卡才能更換造型。" ); 
 			} 
 		} 
 	}		 
 } 
