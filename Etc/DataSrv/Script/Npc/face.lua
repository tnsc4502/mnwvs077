require "./DataSrv/Script/sysDef"

----Henesys VIP plastic surgery
function s_face_henesys1() {
  teye = ( target->getFace() / 100 ) % 10 * 100;
	if ( target->getGender() == 0 ) 
	{
	  changeFace1 = 20000 + teye;
	  changeFace2 = 20001 + teye;
	  changeFace3 = 20002 + teye;
	  changeFace4 = 20003 + teye;
	  changeFace5 = 20004 + teye;
	  changeFace6 = 20005 + teye;
	  changeFace7 = 20006 + teye;
	  changeFace8 = 20007 + teye;
 	  changeFace9 = 20008 + teye;
    changeFace10 = 20012 + teye;
    changeFace11 = 20014 + teye;
	  mFace = self->askAvatar( "以下是我們提供的臉型範例，如果你有攜帶#b#t5152001##k，那你就可以進行手術並且更換成你喜歡的樣式。", 5152001, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
	}
	else if ( target->getGender() == 1 ) 
	{
	  changeFace1 = 21000 + teye;
	  changeFace2 = 21001 + teye;
	  changeFace3 = 21002 + teye;
	  changeFace4 = 21003 + teye;
	  changeFace5 = 21004 + teye;
	  changeFace6 = 21005 + teye;
	  changeFace7 = 21006 + teye;
	  changeFace8 = 21007 + teye;
    changeFace9 = 21008 + teye;
	  changeFace10 = 21012 + teye;
	  changeFace11 = 21014 + teye;
	  mFace = self->askAvatar( "以下是我們提供的臉型範例，如果你有攜帶#b#t5152001##k，那你就可以進行手術並且更換成你喜歡的樣式。", 5152001, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
	}
	
	if ( mFace == 1 ) self->say( "非常好，手術進行得非常順利。你可以到這邊看看你新的造型。怎麼樣，非常棒對吧？我就知道！如果你還想進行整形手術，請再來找我！" );
	else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
	else if ( mFace == -3 ) self->say( "Hum... Parece que temos um problema aqui no hospital, e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
	else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
}

----Henesys regular plastic surgery
function s_face_henesys2() {
	nRet = self->askYesNo( "歡迎光臨，我是這裡的實習醫生，如果你有#b#t5152000##k，就可以替你進行整形手術，如何，想要進行整型嗎？" );
	if ( nRet == 0 ) self->say( "沒關係...但是等你考慮清楚後，你可以再回來找我。" );
	else if ( nRet == 1 ) {
    teye = ( target->getFace() / 100 ) % 10 * 100;
		if ( target->getGender() == 0 ) 
		{
  	  changeFace1 = 20000 + teye;
  	  changeFace2 = 20001 + teye;
  	  changeFace3 = 20002 + teye;
  	  changeFace4 = 20003 + teye;
  	  changeFace5 = 20004 + teye;
  	  changeFace6 = 20005 + teye;
  	  changeFace7 = 20006 + teye;
  	  changeFace8 = 20007 + teye;
	  	changeFace9 = 20008 + teye;
	  	changeFace10 = 20012 + teye;
	  	changeFace11 = 20014 + teye;
		  mFace = self->makeRandAvatar( 5152000, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}
		else if ( target->getGender() == 1 ) 
		{
  	  changeFace1 = 21000 + teye;
  	  changeFace2 = 21001 + teye;
  	  changeFace3 = 21002 + teye;
  	  changeFace4 = 21003 + teye;
  	  changeFace5 = 21004 + teye;
  	  changeFace6 = 21005 + teye;
  	  changeFace7 = 21006 + teye;
  	  changeFace8 = 21007 + teye;
	  	changeFace9 = 21008 + teye;
	  	changeFace10 = 21012 + teye;
	  	changeFace11 = 21014 + teye;
		  mFace = self->makeRandAvatar( 5152000, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}

		if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
		else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico deste lugar. Desculpa dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe, mas h?um problema aqui no hospital e temo que tenha que parar a cirurgia imediatamente. Por favor, volte depois." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
	}
}

----Orbis VIP plastic surgery
function s_face_orbis1() {
  teye = ( target->getFace() / 100 ) % 10 * 100;
	if ( target->getGender() == 0 ) 
	{
	  changeFace1 = 20000 + teye;
	  changeFace2 = 20001 + teye;
	  changeFace3 = 20002 + teye;
	  changeFace4 = 20003 + teye;
	  changeFace5 = 20004 + teye;
	  changeFace6 = 20005 + teye;
	  changeFace7 = 20006 + teye;
	  changeFace8 = 20007 + teye;
    changeFace9 = 20008 + teye;
	  changeFace10 = 20012 + teye;
	  changeFace11 = 20014 + teye;
    mFace = self->askAvatar( "歡迎，歡迎。對現在的造型感到厭倦了嗎？沒關係，只要你有#b#t5152005##k，我就可以幫你進行手術並且改造成以下你所喜歡的造型，如何？需要進行手術嗎？", 5152005, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
  }
	else if ( target->getGender() == 1 )	{
	  changeFace1 = 21000 + teye;
	  changeFace2 = 21001 + teye;
	  changeFace3 = 21002 + teye;
	  changeFace4 = 21003 + teye;
	  changeFace5 = 21004 + teye;
	  changeFace6 = 21005 + teye;
	  changeFace7 = 21006 + teye;
	  changeFace8 = 21007 + teye;
    changeFace9 = 21008 + teye;
    changeFace10 = 21012 + teye;
    changeFace11 = 21014 + teye;
	  mFace = self->askAvatar( "歡迎，歡迎。對現在的造型感到厭倦了嗎？沒關係，只要你有#b#t5152005##k，我就可以幫你進行手術並且改造成以下你所喜歡的造型，如何？需要進行手術嗎？", 5152005, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
	}
	
	if ( mFace == 1 ) self->say( "非常好，手術進行得非常順利。你可以到這邊看看你新的造型。怎麼樣，非常棒對吧？我就知道！如果你還想進行整形手術，請再來找我！" );
	else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
	else if ( mFace == -3 ) self->say( "Hum... Parece que temos um problema aqui no hospital e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
	else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
}

----Orbis regular plastic surgery
function s_face_orbis2() {
	nRet = self->askYesNo( "歡迎光臨，我是這裡的實習醫生，如果你有#b#t5152004##k，就可以替你進行整形手術，如何，想要進行整型嗎？" );
	if ( nRet == 0 ) self->say( "沒關係...但是等你考慮清楚後，你可以再回來找我。");
	else if ( nRet == 1 ) {
    teye = ( target->getFace() / 100 ) % 10 * 100;
		if ( target->getGender() == 0 ) 
		{
  	  changeFace1 = 20000 + teye;
  	  changeFace2 = 20001 + teye;
  	  changeFace3 = 20002 + teye;
  	  changeFace4 = 20003 + teye;
  	  changeFace5 = 20004 + teye;
  	  changeFace6 = 20005 + teye;
  	  changeFace7 = 20006 + teye;
  	  changeFace8 = 20007 + teye;
      changeFace9 = 20008 + teye;
	  	changeFace10 = 20012 + teye;
	  	changeFace11 = 20014 + teye;
		  mFace = self->makeRandAvatar( 5152004, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}
		else if ( target->getGender() == 1 ) 
		{
  	  changeFace1 = 21000 + teye;
  	  changeFace2 = 21001 + teye;
  	  changeFace3 = 21002 + teye;
  	  changeFace4 = 21003 + teye;
  	  changeFace5 = 21004 + teye;
  	  changeFace6 = 21005 + teye;
  	  changeFace7 = 21006 + teye;
  	  changeFace8 = 21007 + teye;
	  	changeFace9 = 21008 + teye;
	  	changeFace10 = 21012 + teye;
	  	changeFace11 = 21014 + teye;
		  mFace = self->makeRandAvatar( 5152004, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}

		if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
		else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe-me dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
		else if ( mFace == -3 ) self->say( "Hum... Parece que temos um problema aqui no hospital e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
	}
}

----Ludibrium VIP plastic surgery
function s_face_ludi1() {
  teye = ( target->getFace() / 100 ) % 10 * 100;
	if ( target->getGender() == 0 ) 
	{
	  changeFace1 = 20000 + teye;
	  changeFace2 = 20001 + teye;
	  changeFace3 = 20002 + teye;
	  changeFace4 = 20003 + teye;
	  changeFace5 = 20004 + teye;
	  changeFace6 = 20005 + teye;
	  changeFace7 = 20006 + teye;
	  changeFace8 = 20007 + teye;
  	  changeFace9 = 20008 + teye;
  	  changeFace10 = 20012 + teye;
	  changeFace11 = 20014 + teye;
    mFace = self->askAvatar( "以下是我們提供的臉型範例，如果你有攜帶#b#t5152007##k，那你就可以進行手術並且更換成你喜歡的樣式。", 5152007, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
  }
	else if ( target->getGender() == 1 )	{
	  changeFace1 = 21000 + teye;
	  changeFace2 = 21001 + teye;
	  changeFace3 = 21002 + teye;
	  changeFace4 = 21003 + teye;
	  changeFace5 = 21004 + teye;
	  changeFace6 = 21005 + teye;
	  changeFace7 = 21006 + teye;
	  changeFace8 = 21007 + teye;
	  changeFace9 = 21008 + teye;
	  changeFace10 = 21012 + teye;
	  changeFace11 = 21014 + teye;
	  mFace = self->askAvatar( "以下是我們提供的臉型範例，如果你有攜帶#b#t5152007##k，那你就可以進行手術並且更換成你喜歡的樣式。", 5152007, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
	}
	
	if ( mFace == 1 ) self->say( "非常好，手術進行得非常順利。你可以到這邊看看你新的造型。怎麼樣，非常棒對吧？我就知道！如果你還想進行整形手術，請再來找我！" );
	else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe-me dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
	else if ( mFace == -3 ) self->say( "Hum... Parece que temos um problema aqui no hospital e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
	else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
}

----Ludibrium regular plastic surgery
function s_face_ludi2() {
	nRet = self->askYesNo( "歡迎光臨，我是這裡的實習醫生，如果你有#b#t5152006##k，就可以替你進行整形手術，如何，想要進行整型嗎？" );
	if ( nRet == 0 ) self->say( "等你考慮清楚以後再回來找我。");
	else if ( nRet == 1 ) {
    teye = ( target->getFace() / 100 ) % 10 * 100;
		if ( target->getGender() == 0 ) 
		{
  	  changeFace1 = 20000 + teye;
  	  changeFace2 = 20001 + teye;
  	  changeFace3 = 20002 + teye;
  	  changeFace4 = 20003 + teye;
  	  changeFace5 = 20004 + teye;
  	  changeFace6 = 20005 + teye;
  	  changeFace7 = 20006 + teye;
  	  changeFace8 = 20007 + teye;
      changeFace9 = 20008 + teye;
      changeFace10 = 20012 + teye;
	  	changeFace11 = 20014 + teye;
		  mFace = self->makeRandAvatar( 5152006, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}
		else if ( target->getGender() == 1 ) 
		{
  	  changeFace1 = 21000 + teye;
  	  changeFace2 = 21001 + teye;
  	  changeFace3 = 21002 + teye;
  	  changeFace4 = 21003 + teye;
  	  changeFace5 = 21004 + teye;
  	  changeFace6 = 21005 + teye;
  	  changeFace7 = 21006 + teye;
  	  changeFace8 = 21007 + teye;
	  	changeFace9 = 21008 + teye;
	  	changeFace10 = 21012 + teye;
	  	changeFace11 = 21014 + teye;
		  mFace = self->makeRandAvatar( 5152006, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}


		if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
		else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe pela inconveni瘽cia, mas parece que temos um problema aqui no hospital e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "對不起，如果沒有攜帶會員卡，我就無法為你服務。" );
	}
}
--****** Wedding : Plastic Surgery - Dr. 90212 - VIP			18
function s_face_wedding1() {
 
		teye = ( target->getFace() / 100 ) % 10 * 100;

		if ( target->getGender() == 0 ) 	{

			  changeFace1 = 20018 + teye;
			  changeFace2 = 20019 + teye;

			  changeFace3 = 20000 + teye;
			  changeFace4 = 20001 + teye;
			  changeFace5 = 20003 + teye;
			  changeFace6 = 20004 + teye;
			  changeFace7 = 20005 + teye;
			  changeFace8 = 20006 + teye;
			  changeFace9 = 20007 + teye;  
			  changeFace10 = 20008 + teye;  

			  mFace = self->askAvatar( "有你喜歡的造型嗎？如果你有攜帶#b#t5152022##k，我就可以幫你進行手術並且整形成你所夢寐以求的樣子！", 5152022, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10);
		}
		else if ( target->getGender() == 1 ) {

			  changeFace1 = 21018 + teye;
			  changeFace2 = 21019 + teye;

			  changeFace3 = 21001 + teye;
			  changeFace4 = 21002 + teye;
			  changeFace5 = 21003 + teye;
			  changeFace6 = 21004 + teye;
			  changeFace7 = 21005 + teye;
			  changeFace8 = 21006 + teye;
			  changeFace9 = 21007 + teye;
			  changeFace10 = 21012 + teye;
	
			 mFace = self->askAvatar( "有你喜歡的造型嗎？如果你有攜帶#b#t5152022##k，我就可以幫你進行手術並且整形成你所夢寐以求的樣子！", 5152022, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10);
		}
	
		if ( mFace == 1 ) self->say( "手術進行得非常順利，你的外型現在變得比以前好看多了！你還滿意嗎？如果以後還有需要服務，可以再來找我！" );
		else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe-me dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
		else if ( mFace == -3 ) self->say( "Hum... Parece que temos um problema aqui no hospital e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );

}


--****** Wedding : Script: Plastic Surgery - Intern Shakihands - REG	19
function s_face_wedding2() {

		nRet = self->askYesNo( "想要進行整形手術嗎？只要你有#b#t5152021##k，我們馬上就可以開始！" );

		if ( nRet == 0 ) self->say( "沒關係...但是等你考慮清楚後，你可以再回來找我。" );
		else if ( nRet == 1 ) {
			teye = ( target->getFace() / 100 ) % 10 * 100;
			if ( target->getGender() == 0 ) 	{
			  changeFace1 = 20018 + teye;
			  changeFace2 = 20019 + teye;

			  changeFace3 = 20000 + teye;
			  changeFace4 = 20001 + teye;
			  changeFace5 = 20003 + teye;
			  changeFace6 = 20004 + teye;
			  changeFace7 = 20005 + teye;
			  changeFace8 = 20006 + teye;
			  changeFace9 = 20007 + teye;  
			  changeFace10 = 20008 + teye;  

		  mFace = self->makeRandAvatar( 5152021, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10 );
		}
		else if ( target->getGender() == 1 ) {
			  changeFace1 = 21018 + teye;
			  changeFace2 = 21019 + teye;

			  changeFace3 = 21001 + teye;
			  changeFace4 = 21002 + teye;
			  changeFace5 = 21003 + teye;
			  changeFace6 = 21004 + teye;
			  changeFace7 = 21005 + teye;
			  changeFace8 = 21006 + teye;
			  changeFace9 = 21007 + teye;
			  changeFace10 = 21012 + teye;

		  mFace = self->makeRandAvatar( 5152021, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10 );
		}

		if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
		else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe-me dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe, mas h?um problema aqui no hospital e temo que tenha que parar a cirurgia imediatamente. Por favor, volte depois." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
	}
}

function s_lens_henesys1()
{
  nRet = self->askMenu( "嗨，你好，對自己的外型不滿意嗎？如果你有攜帶#b#t5152010##k或者#b#t5152013##k，我就可以提供你整形服務讓你可以變更換眼睛的顏色，如何？需要哪一種服務呢？\r\n#b#L0# 變更眼睛眼色 (使用一般整形券)#l\r\n#L1# 變更眼睛眼色 (使用VIP整形券)#l" ); 
  if ( nRet == 0 )
  {
    nRet1 = self->askYesNo( "想要進行造型更換嗎？如果你有攜帶#b#t5152010##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
    if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我。" );
    else if ( nRet1 == 1 ) 
    {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 400;
      changeEye4 = tface + 500;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

			mFace = self->makeRandAvatar( 5152010, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
			else if ( mFace == -3 ) self->say( "Desculpe-me, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
    }
  } else if ( nRet == 1 )
  {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 400;
      changeEye4 = tface + 500;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

  	  mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152013, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
  }
}

function s_lens_orbis1()
{
  nRet = self->askMenu( "嗨，你好，對自己的外型不滿意嗎？如果你有攜帶#b#t5152011##k或者#b#t5150104##k，我就可以提供你整形服務讓你可以變更換眼睛的顏色，如何？需要哪一種服務呢？\r\n#b#L0# 變更眼睛眼色 (使用一般整形券)#l\r\n#L1# 變更眼睛眼色 (使用VIP整形券)#l" );
  if ( nRet == 0 )
  {
    nRet1 = self->askYesNo( "想要進行造型更換嗎？如果你有攜帶#b#t5152011##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
    if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我！" );
    else if ( nRet1 == 1 ) 
    {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 200;
      changeEye4 = tface + 400;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

			mFace = self->makeRandAvatar( 5152011, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "非常好，手術進行得非常順利。你可以到這邊看看你新的造型。怎麼樣，非常棒對吧？我就知道！如果你還想進行整形手術，請再來找我！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
    }
	} 
	else if ( nRet == 1 )
  {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 200;
      changeEye4 = tface + 400;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

  	  mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152014, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "非常好，手術進行得非常順利。你可以到這邊看看你新的造型。怎麼樣，非常棒對吧？我就知道！如果你還想進行整形手術，請再來找我！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
  }
}

function s_lens_ludi1()
{
  nRet = self->askMenu( "你好，歡迎來到天空之城整形手術中心！如果對自己的外表不滿意，在這裡將可以進行整型服務。如果你有#b#t5152012##k或者#b#t5152015##k，我就可以替你進行手術並且將你整容成你喜歡的樣子，怎麼樣，需要哪一種服務呢？\r\n#b#L0# 整形 (使用一般整形券)#l\r\n#L1# 整形 (使用高級整形券)#l" );
  if ( nRet == 0 )
  {
    nRet1 = self->askYesNo( "想要進行造型更換嗎？如果你有攜帶#b#t5152012##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
    if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我！" );
    else if ( nRet1 == 1 ) 
    {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 300;
      changeEye4 = tface + 400;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

			mFace = self->makeRandAvatar( 5152012, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc? Desculpe." );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
    }
	} 
	else if ( nRet == 1 )
  {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 300;
      changeEye4 = tface + 400;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

  	  mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152015, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc? Desculpe." );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
 }
}

function s_lens_wedding1()	{
  nRet = self->askMenu( "嗯...歡迎光臨。如果對自己的外表不滿意，在這裡將可以進行整型服務。如果你有#b#t5152025##k或者#b#t5152026##k，我就可以替你進行手術並且將你整容成你喜歡的樣子，怎麼樣，需要哪一種服務呢？\r\n#b#L0# 整形 (使用一般整形券)#l\r\n#L1# 整形 (使用高級整形券)#l" );
  if ( nRet == 0 )
  {
    nRet1 = self->askYesNo( "想要進行造型更換嗎？如果你有攜帶#b#t5152025##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
    if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我！" );
    else if ( nRet1 == 1 ) 
    {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 300;
      changeEye4 = tface + 400;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

			mFace = self->makeRandAvatar( 5152025, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc? Desculpe." );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
    }
	} 
	else if ( nRet == 1 )
  {
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
      changeEye1 = tface;
      changeEye2 = tface + 100;
      changeEye3 = tface + 300;
      changeEye4 = tface + 400;
      changeEye5 = tface + 600;
      changeEye6 = tface + 700;

  	  mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152026, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc? Desculpe." );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
 }
}

----NLC VIP plastic surgery
function s_NLC_FaceVip() {
  teye = ( target->getFace() / 100 ) % 10 * 100;
	if ( target->getGender() == 0 )  {
	  --changeFace1 = 20023 + teye;
	  changeFace2 = 20012 + teye;
	  changeFace3 = 20000 + teye;
	  changeFace4 = 20001 + teye;
	  changeFace5 = 20002 + teye;
	  changeFace6 = 20003 + teye;
	  changeFace7 = 20004 + teye;
	  changeFace8 = 20005 + teye;
	  changeFace9 = 20006 + teye;
	  changeFace10 = 20008 + teye;
    	
	  mFace = self->askAvatar( "以下是我們提供的臉型範例，如果你有攜帶#b#t5152034##k，那你就可以進行手術並且更換成你喜歡的樣式。", 5152034, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10 );
	}
	else if ( target->getGender() == 1 ) 	{
	  --changeFace1 = 21022 + teye;
	  changeFace2 = 21016 + teye;
	  changeFace3 = 21001 + teye;
	  changeFace4 = 21002 + teye;
	  changeFace5 = 21003 + teye;
	  changeFace6 = 21004 + teye;
	  changeFace7 = 21005 + teye;
	  changeFace8 = 21006 + teye;
	  changeFace9 = 21008 + teye;
    changeFace10 = 21012 + teye;
	  
	  mFace = self->askAvatar( "以下是我們提供的臉型範例，如果你有攜帶#b#t5152034##k，那你就可以進行手術並且更換成你喜歡的樣式。", 5152034, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10 );
	}
	
	if ( mFace == 1 ) self->say( "非常好，手術進行得非常順利。你可以到這邊看看你新的造型。怎麼樣，非常棒對吧？我就知道！如果你還想進行整形手術，請再來找我！" );
	else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico para este lugar... Desculpe dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
	else if ( mFace == -3 ) self->say( "Hum... Parece que temos um problema aqui no hospital e sinto que n緌 posso continuar o procedimento imediatamente. Por favor, volte depois." );
	else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
}

----NLC regular plastic surgery
function s_NLC_FaceExp() {
	nRet = self->askYesNo( "歡迎光臨，我是這裡的實習醫生，如果你有#b#t5152033##k，就可以替你進行整形手術，如何，想要進行整型嗎？" );
	if ( nRet == 0 ) self->say( "沒關係...但是等你考慮清楚後，你可以再回來找我。" );
	else if ( nRet == 1 ) {
    teye = ( target->getFace() / 100 ) % 10 * 100;
		if ( target->getGender() == 0 ) {
		
	  	  changeFace1 = 20023 + teye;
		  --changeFace2 = 20012 + teye;
		  changeFace3 = 20000 + teye;
		  changeFace4 = 20001 + teye;
		  changeFace5 = 20002 + teye;
		  changeFace6 = 20003 + teye;
		  changeFace7 = 20004 + teye;
		  changeFace8 = 20005 + teye;
		  changeFace9 = 20006 + teye;
		  changeFace10 = 20008 + teye;
		  
		  mFace = self->makeRandAvatar( 5152033, changeFace1, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10 );
		}
		else if ( target->getGender() == 1 ) {
		  
		  changeFace1 = 21022 + teye;
		  --/changeFace2 = 21016 + teye;
		  changeFace3 = 21001 + teye;
		  changeFace4 = 21002 + teye;
		  changeFace5 = 21003 + teye;
		  changeFace6 = 21004 + teye;
		  changeFace7 = 21005 + teye;
		  changeFace8 = 21006 + teye;
		  changeFace9 = 21008 + teye;
	      changeFace10 = 21012 + teye;
	  
		  mFace = self->makeRandAvatar( 5152033, changeFace1, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10);
		}

		if ( mFace == 1 ) self->say( "完成了！怎麼樣？雖然不是非常完美，但是仍然非常的適合你對吧？如果你厭倦了這個造型，可以再回來找我！" );
		else if ( mFace == -1 ) self->say( "Hum... Parece que voc?n緌 tem o cupom espec璗ico deste lugar. Desculpa dizer isso, mas, sem o cupom, nada de cirurgia pl嫳tica para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe, mas h?um problema aqui no hospital e temo que tenha que parar a cirurgia neste minuto. Por favor, volte depois." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "嗯...需要有會員卡才能夠進行整形手術。" );
	}
}

function s_NLC_LensVip(){

	  tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
	  
	  changeEye1 = tface;
	  changeEye2 = tface + 100;
	  changeEye3 = tface + 200;
	  changeEye4 = tface + 300;
	  changeEye5 = tface + 400;
	  changeEye6 = tface + 500;
	  changeEye7 = tface + 600;
	  changeEye8 = tface + 700;

  	  mFace = self->askAvatar( "歡迎光臨，我們提供了多種不同的造型整形服務。只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152036, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6, changeEye7, changeEye8 );

	if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
	else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
	else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
	else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
 
}

function s_NLC_LensExp(){

    nRet1 = self->askYesNo( "你好，歡迎光臨。想要進行造型更換嗎？如果你有攜帶#b#t5152035##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
    if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我。" );
    else if ( nRet1 == 1 )     {
	
      tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
	  
      changeEye1 = tface;
	  changeEye2 = tface + 100;
	  changeEye3 = tface + 200;
	  changeEye4 = tface + 300;
	  changeEye5 = tface + 400;
	  changeEye6 = tface + 500;
	  changeEye7 = tface + 600;
	  changeEye8 = tface + 700;

		mFace = self->makeRandAvatar( 5152035, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6, changeEye7, changeEye8 );

		if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
		else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
		else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
    }
  
}

--鼠葵 堅晝 撩??諼婁
function s_face_mureung1() {

	nRet = self->askMenu( "歡迎來到武陵整形手術中心！覺得自己的造型不夠吸引人嗎？只要你有#b#t5152028##k或者#b#t5152041##k，我就可以替你進行手術並且將你整容成你喜歡的樣子，怎麼樣，需要哪一種服務呢？\r\n#b#L0# 更換臉型 (使用高級整形卡)#l\r\n#L1# 更換眼睛顏色 (使用高級整形卡)#l" );
  if ( nRet == 0 )  {
		teye = ( target->getFace() / 100 ) % 10 * 100;
		if ( target->getGender() == 0 ) 
		{
			changeFace1 = 20010 + teye;
			changeFace2 = 20000 + teye;
			changeFace3 = 20002 + teye;
			changeFace4 = 20004 + teye;
			changeFace5 = 20005 + teye;
			changeFace6 = 20006 + teye;
			changeFace7 = 20007 + teye;
			changeFace8 = 20012 + teye;
			changeFace9 = 20009 + teye;

			mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152028, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9 );
		}
		else if ( target->getGender() == 1 ) 
		{
			changeFace1 = 21011 + teye;
			changeFace2 = 21000 + teye;
			changeFace3 = 21002 + teye;
			changeFace4 = 21003 + teye;
			changeFace5 = 21005 + teye;
			changeFace6 = 21006 + teye;
			changeFace7 = 21008 + teye;
			changeFace8 = 21012 + teye;
			changeFace9 = 21009 + teye;		
			mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152028, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9 );
		}

		if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
		else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
		else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
  }
	
	if ( nRet == 1 )  {
		tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
		changeEye1 = tface;
		changeEye2 = tface + 100;
		changeEye3 = tface + 200;
		changeEye4 = tface + 300;
		changeEye5 = tface + 400;
		changeEye6 = tface + 500;
		changeEye7 = tface + 600;
		changeEye8 = tface + 700;
		mFace = self->askAvatar( "只要你有攜帶高級會員卡，你就可以整形成以下你所喜歡的任意的造型：", 5152041, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6, changeEye7, changeEye8 );
		if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
		else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
		else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
		else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
	}
}

----鼠葵 橾奩 撩??諼婁
function s_face_mureung2() {
	nRet = self->askMenu( "歡迎光臨，我是這裡的實習醫生，只要你有攜帶#b#t5152027##k或者#b#t5152042##k我就可以為你進行整形服務。\r\n#b#L0# 更換臉型 (使用一般整形卡)#l\r\n#L1# 更換眼睛顏色 (使用一般整形卡)#l" );
  if ( nRet == 0 ) 	{
		nRet1 = self->askYesNo( "想要進行造型更換嗎？如果你有攜帶#b#t5152027##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
		if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我。");
		else if ( nRet1 == 1 ) {
			teye = ( target->getFace() / 100 ) % 10 * 100;
			if ( target->getGender() == 0 ) 
			{
				changeFace1 = 20009 + teye;
				changeFace2 = 20000 + teye;
				changeFace3 = 20002 + teye;
				changeFace4 = 20004 + teye;
				changeFace5 = 20005 + teye;
				changeFace6 = 20006 + teye;
				changeFace7 = 20007 + teye;
				changeFace8 = 20012 + teye;
			
				mFace = self->makeRandAvatar( 5152027, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8 );
			}
			else if ( target->getGender() == 1 ) 
			{
				changeFace1 = 21009 + teye;
				changeFace2 = 21000 + teye;
				changeFace3 = 21002 + teye;
				changeFace4 = 21003 + teye;
				changeFace5 = 21005 + teye;
				changeFace6 = 21006 + teye;
				changeFace7 = 21008 + teye;
				changeFace8 = 21012 + teye;
			
				mFace = self->makeRandAvatar( 5152027, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8 );
			}
				
			if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
		}
	}
	
	if ( nRet == 1 ) {	
		nRet1 = self->askYesNo( "想要進行造型更換嗎？如果你有攜帶#b#t5152042##k我就可以幫你#b隨機#k更換造型，怎麼樣，要進行手術嗎？" );
    if ( nRet1 == 0 ) self->say( "沒關係...但是等你考慮清楚後如果決定要進行手術，請再回來找我。" );
    else if ( nRet1 == 1 ) {
			tface = target->getFace() - ( target->getFace() / 100 ) % 10 * 100;
			changeEye1 = tface;
			changeEye2 = tface + 100;
			changeEye3 = tface + 200;
			changeEye4 = tface + 300;
			changeEye5 = tface + 400;
			changeEye6 = tface + 500;
			changeEye7 = tface + 600;
			changeEye8 = tface + 700;

			mFace = self->makeRandAvatar( 5152042, changeEye1, changeEye2, changeEye3, changeEye4, changeEye5, changeEye6, changeEye7, changeEye8 );

			if ( mFace == 1 ) self->say( "完成了！怎麼樣，對於這個新的造型還滿意吧？並沒有這麼差對吧！" );
			else if ( mFace == -1 ) self->say( "Desculpe, mas acho que agora voc?n緌 tem o cupom de nossas lentes de contato. Sem o cupom, sinto muito, mas n緌 posso fazer isso para voc?" );
			else if ( mFace == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de fazer lentes de contato n緌 est?funcionando agora. Por favor, volte depois. Me desculpe, de verdade!" );
			else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，需要有會員卡才能夠進行整形手術。" );
    }
	}
}
----澗諦 堅晝 撩??熱獎
function s_face_shouwa1() {
  teye = ( target->getFace() / 100 ) % 10 * 100;
	if ( target->getGender() == 0 ) 
  {
  	changeFace1 = 20002 + teye;
	  changeFace2 = 20003 + teye;
	  changeFace3 = 20004 + teye;
	  changeFace4 = 20007 + teye;
	  changeFace5 = 20008 + teye;
	  changeFace6 = 20009 + teye;
	  changeFace7 = 20010 + teye;
	  changeFace8 = 20011 + teye;
  	changeFace9 = 20013 + teye;
	  changeFace10 = 20016 + teye;
	  changeFace11 = 20017 + teye;
    mFace = self->askAvatar( "因為醫學的進步，我們可以立即為你進行整形。只要你有攜帶#b#t5152009##k，你就可以整形成以下你所喜歡的任意的造型。", 5152009, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
	}
	else if ( target->getGender() == 1 ) 
  {
	  changeFace1 = 21003 + teye;
	  changeFace2 = 21003 + teye;
	  changeFace3 = 21004 + teye;
	  changeFace4 = 21005 + teye;
	  changeFace5 = 21008 + teye;
	  changeFace6 = 21009 + teye;
	  changeFace7 = 21010 + teye;
	  changeFace8 = 21011 + teye;
  	changeFace9 = 21013 + teye;
	  changeFace10 = 21016 + teye;
	  changeFace11 = 21017 + teye;
	  mFace = self->askAvatar( "因為醫學的進步，我們可以立即為你進行整形。只要你有攜帶#b#t5152009##k，你就可以整形成以下你所喜歡的任意的造型。", 5152009, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
	}
	if ( mFace == 1 ) self->say( "手術已經完成，我必須說這真的非常的適合你，你覺得呢？如果你厭倦了這個造型，請再回來進行整型吧。" );
	else if ( mFace == -1 ) self->say( "I'm afraid you don't have our designated plastic surgery coupon. I'm sorry, but without the coupon, there's no plastic surgery for you." );
	else if ( mFace == -3 ) self->say( "I'm afraid we have a problem here at the hospital and I can't go on with the procedure right this minute. Please come back later." );
	else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，但是你必須攜帶會員卡才能夠進行手術。" );
}

----澗諦 橾奩 撩??熱獎
function s_face_shouwa2() {
	nRet = self->askYesNo( "如果你使用一般的整形會員卡，那麼你的外型將會#b隨機改變#k，也就是說一切靠運氣...這樣一來你還想使用#b#t5152008##k來進行手術嗎？" );
	if ( nRet == 0 ) self->say( "我瞭解，你可以再多多考慮，等你做好決定以後，可以再回來找我。" );
	else if ( nRet == 1 ) {
    teye = ( target->getFace() / 100 ) % 10 * 100;
		if ( target->getGender() == 0 ) 
	  {
  	  changeFace1 = 20002 + teye;
			changeFace2 = 20003 + teye;
			changeFace3 = 20004 + teye;
			changeFace4 = 20007 + teye;
			changeFace5 = 20008 + teye;
			changeFace6 = 20009 + teye;
			changeFace7 = 20010 + teye;
			changeFace8 = 20011 + teye;
  	  changeFace9 = 20013 + teye;	
	  	changeFace10 = 20016 + teye;
			changeFace11 = 20017 + teye;
      mFace = self->makeRandAvatar( 5152008, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}
		else if ( target->getGender() == 1 ) 
    {
			changeFace1 = 21003 + teye;
			changeFace2 = 21003 + teye;
			changeFace3 = 21004 + teye;
			changeFace4 = 21005 + teye;
			changeFace5 = 21008 + teye;
			changeFace6 = 21009 + teye;
			changeFace7 = 21010 + teye;
			changeFace8 = 21011 + teye;
			changeFace9 = 21013 + teye;
			changeFace10 = 21016 + teye;
			changeFace11 = 21017 + teye;
			mFace = self->makeRandAvatar( 5152008, changeFace1, changeFace2, changeFace3, changeFace4, changeFace5, changeFace6, changeFace7, changeFace8, changeFace9, changeFace10, changeFace11 );
		}
		if ( mFace == 1 ) self->say( "手術已經完成，看起來還不錯吧？如果你厭倦了這個造型，請再回來進行整型吧。" );
		else if ( mFace == -1 ) self->say( "I'm afraid you don't have our designated regular plastic surgery coupon. I'm sorry, but without the regular coupon, there's no plastic surgery for you." );
		else if ( mFace == -3 ) self->say( "I'm afraid we have a problem here at the hospital and I can't go on with the procedure right this minute. Please come back later." );
		else if ( mFace == 0 or mFace == -2 ) self->say( "非常抱歉，但是你必須攜帶會員卡才能夠進行手術。" );
	}
 }
