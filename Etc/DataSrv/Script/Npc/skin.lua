require "./DataSrv/Script/sysDef"

--// Henesys Skin-care
function s_skin_henesys1() {
	self->sayNext( "歡迎光臨，想要更換你的皮膚顏色來讓自己看起來更健康嗎？沒問題，只要你有攜帶#b#t5153000##k，我就可以幫你把皮膚顏色換成你所喜歡的樣式！" );

	mHair = self->askAvatar( "選擇你所喜歡的皮膚顏色，接著我們馬上就可以進行手術：", 5153000, 0, 1, 2, 3, 4 );
	
	if ( mHair == 1 ) self->say( "怎麼樣？新的膚色非常適合你對吧！如果還需要服務，歡迎再回來找我。" );
	else if ( mHair == -1 ) self->say( "Hum... voc?n緌 tem o cupom de tratamento de pele que precisa para receber o tratamento. Desculpe, mas n緌 podemos fazer." );
	else if ( mHair == -3 ) self->say( "Me desculpe. Parece que nossa m嫭uina de tratamento de pele n緌 est?funcionando no momento. Por favor, volte mais tarde." );
	else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你沒有攜帶護膚會員卡，我無法為你服務。" );
}

--// Orbis Skin-care
function s_skin_orbis1() {
	self->sayNext( "歡迎光臨，想要更換你的皮膚顏色來讓自己看起來更健康嗎？沒問題，只要你有攜帶#b#t5153001##k，我就可以幫你把皮膚顏色換成你所喜歡的樣式！" );

	mHair = self->askAvatar( "選擇你所喜歡的皮膚顏色，接著我們馬上就可以進行手術：", 5153001, 0, 1, 2, 3, 4 );
	
	if ( mHair == 1 ) self->say( "怎麼樣？新的膚色非常適合你對吧！如果還需要服務，歡迎再回來找我。" );
	else if ( mHair == -1 ) self->say( "Parece que voc?n緌 tem o cupom que precisa para receber o tratamento. Sinto muito, mas parece que n緌 poderemos fazer." );
	else if ( mHair == -3 ) self->say( "Parece que nossa m嫭uina de tratamento de pele n緌 est?funcionando no momento. Por favor, volte daqui a pouquinho." );
	else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你沒有攜帶護膚會員卡，我無法為你服務。" );
}

--// Ludibrium Skin-care
function s_skin_ludi1() {
	self->sayNext( "歡迎光臨，想要更換你的皮膚顏色來讓自己看起來更健康嗎？沒問題，只要你有攜帶#b#t5153002##k，我就可以幫你把皮膚顏色換成你所喜歡的樣式！" );

	mHair = self->askAvatar( "選擇你所喜歡的皮膚顏色，接著我們馬上就可以進行手術：", 5153002, 0, 1, 2, 3, 4 );
	
	if ( mHair == 1 ) self->say( "怎麼樣？新的膚色非常適合你對吧！如果還需要服務，歡迎再回來找我。" );
	else if ( mHair == -1 ) self->say( "Humm... Acho que voc?n緌 tem nosso cupom de tratamento de pele no momento. Sem o cupom, desculpe, n緌 poderei fazer." );
	else if ( mHair == -3 ) self->say( "Desculpe, mas parece que nossa m嫭uina de tratamento de pele n緌 est?funcionando no momento. Por favor, volte mais tarde. Me desculpe, de verdade!" );
	else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你沒有攜帶護膚會員卡，我無法為你服務。" );
}

--// NLC Skin-care
function s_NLC_Skin() {
	self->sayNext( "歡迎光臨，想要更換你的皮膚顏色來讓自己看起來更健康嗎？沒問題，只要你有攜帶#b#t5153009##k，我就可以幫你把皮膚顏色換成你所喜歡的樣式！" );

	mHair = self->askAvatar( "選擇你所喜歡的皮膚顏色，接著我們馬上就可以進行手術：", 5153009, 0, 1, 2, 3, 4 );
	
	if ( mHair == 1 ) self->say( "怎麼樣？新的膚色非常適合你對吧！如果還需要服務，歡迎再回來找我。" );
	else if ( mHair == -1 ) self->say( "Hum... voc?n緌 tem o cupom de tratamento de pele que precisa para receber o tratamento. Desculpe, mas n緌 podemos fazer." );
	else if ( mHair == -3 ) self->say( "Me desculpe. Parece que nossa m嫭uina de tratamento de pele n緌 est?funcionando no momento. Por favor, volte mais tarde." );
	else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你沒有攜帶護膚會員卡，我無法為你服務。" );
}


--//鼠葵 ??睡婦葬 憧
function s_skin_mureung1() {
	self->sayNext( "歡迎光臨，想要更換你的皮膚顏色來讓自己看起來更健康嗎？沒問題，只要你有攜帶#b#t5153006##k，我就可以幫你把皮膚顏色換成你所喜歡的樣式！" );

	mHair = self->askAvatar( "選擇你所喜歡的皮膚顏色，接著我們馬上就可以進行手術：", 5153006, 0, 1, 2, 3, 4 );
	
	if ( mHair == 1 ) self->say( "怎麼樣？新的膚色非常適合你對吧！如果還需要服務，歡迎再回來找我。" );
	else if ( mHair == -1 ) self->say( "Hum... voc?n緌 tem o cupom de tratamento de pele que precisa para receber o tratamento. Desculpe, mas n緌 podemos fazer." );
	else if ( mHair == -3 ) self->say( "Me desculpe. Parece que nossa m嫭uina de tratamento de pele n緌 est?funcionando no momento. Por favor, volte mais tarde." );
	else if ( mHair == 0 or mHair == -2 ) self->say( "對不起，你沒有攜帶護膚會員卡，我無法為你服務。" );
}
