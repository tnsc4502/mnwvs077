require "./DataSrv/Script/sysDef"

function party_check() {
	for i = 0, 5 {
		nLevel = target->getPartyMemberLevel( i );
		if ( nLevel == 0 ) {
			self->say( "Seu grupo n緌 possui 6 membros ou nem todos est緌 presentes. Desse jeito n緌 d?" );
			return;
		}
	}
}

function party3_takeawayitem() {
	inven = inventory;

	nItem = inven->itemCount( 4001044 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001044, -nItem );

	nItem = inven->itemCount( 4001045 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001045, -nItem );

	nItem = inven->itemCount( 4001046 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001046, -nItem );

	nItem = inven->itemCount( 4001047 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001047, -nItem );

	nItem = inven->itemCount( 4001048 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001048, -nItem );

	nItem = inven->itemCount( 4001049 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001049, -nItem );

	nItem = inven->itemCount( 4001050 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001050, -nItem );

	nItem = inven->itemCount( 4001051 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001051, -nItem );

	nItem = inven->itemCount( 4001052 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001052, -nItem );

	nItem = inven->itemCount( 4001053 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001053, -nItem );

	nItem = inven->itemCount( 4001054 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001054, -nItem );

	nItem = inven->itemCount( 4001055 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001055, -nItem );

	nItem = inven->itemCount( 4001056 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001056, -nItem );

	nItem = inven->itemCount( 4001057 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001057, -nItem );

	nItem = inven->itemCount( 4001058 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001058, -nItem );

	nItem = inven->itemCount( 4001059 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001059, -nItem );

	nItem = inven->itemCount( 4001060 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001060, -nItem );

	nItem = inven->itemCount( 4001061 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001061, -nItem );

	nItem = inven->itemCount( 4001062 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001062, -nItem );

	nItem = inven->itemCount( 4001063 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001063, -nItem );

	nItem = inven->itemCount( 4001074 );
	if ( nItem > 0 ) ret = inven->exchange( 0, 4001074, -nItem );

	return;
}

function premission_prev() {
--//	party_check;

	wJob = 0;
	mJob = 0;
	aJob = 0;
	tJob = 0;

	for i = 0, 5 {
		nJob = target->getPartyMemberJob( i );

		if ( nJob >= 100 and nJob < 200 ) {
			wJob = wJob + 1;
		} else if ( nJob >= 200 and nJob < 300 ) {
			mJob = mJob + 1;
		} else if ( nJob >= 300 and nJob < 400 ) {
			aJob = aJob + 1;
		} else if ( nJob >= 400 and nJob < 500 ) {
			tJob = tJob + 1;
		}
	}

	if ( wJob > 0 and mJob > 0 and aJob > 0 and tJob > 0 ) {
		return random( 1, 4 );
	}
	return 0;
}


function  premission_decision( mCode ) {
	party_check();

	if ( mCode >= 10 and mCode <= 13 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 110 ) {
				if ( mCode == 10 ) {
					if( nLevel >= 51 and nLevel <= 55 ) return 1;
				}
				else if ( mCode == 11 ) {
					if( nLevel >= 56 and nLevel <= 60 ) return 1;
				}
				else if ( mCode == 12 ) {
					if( nLevel >= 61 and nLevel <= 65 ) return 1;
				}
				else if ( mCode == 13 ) {
					if( nLevel >= 66 and nLevel <= 70 ) return 1;
				}
			}
		}
	} else if ( mCode >= 14 and mCode <= 17 ) {
		for i = 0,5  {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 120 ) {
				if ( mCode == 14 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 15 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 16 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 17 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			} 
		}
	}
	else if ( mCode >= 18 and mCode <= 21 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 130 ) {
				if ( mCode == 18 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 19 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 20 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 21 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 22 and mCode <= 25 ) {
		for i = 0 , 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 210 ) {
				if ( mCode == 22 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 23 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 24 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 25 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 26 and mCode <= 29 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 220 ) {
				if ( mCode == 26 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 27 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 28 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 29 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 30 and mCode <= 33 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 230 ) {
				if ( mCode == 30 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 31 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 32 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 33 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 34 and mCode <= 37 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 310 ) {
				if ( mCode == 34 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 35 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 36 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 37 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 38 and mCode <= 41 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 320 ) {
				if ( mCode == 38 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 39 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 40 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 41 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 42 and mCode <= 45 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 410 ) {
				if ( mCode == 42 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 43 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 44 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 45 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	else if ( mCode >= 46 and mCode <= 49 ) {
		for i = 0, 5 {
			nLevel = target->getPartyMemberLevel( i );
			nJob = target->getPartyMemberJob( i );

			if ( nJob == 420 ) {
				if ( mCode == 46 ) {
					if( nLevel >= 51 and nLevel <= 55 ) {
						return 1;
					}
				}
				else if ( mCode == 47 ) {
					if( nLevel >= 56 and nLevel <= 60 ) {
						return 1;
					}
				}
				else if ( mCode == 48 ) {
					if( nLevel >= 61 and nLevel <= 65 ) {
						return 1;
					}
				}
				else if ( mCode == 49 ) {
					if( nLevel >= 66 and nLevel <= 70 ) {
						return 1;
					}
				}
			}
		}
	}
	return 2;
}

--//?襄捆抯?薑爾
function premission(  nNum ) {
	if ( nNum == 10 ) {
		job = "?釋旼?";
		level = "51~55";
	} else if ( nNum == 11 ) {
		job = "?釋旼?";
		level = "56~60";
	} else if ( nNum == 12 ) {
		job = "?釋旼?";
		level = "61~65";
	} else if ( nNum == 13 ) {
		job = "?釋旼?";
		level = "66~70";
	}
	else if ( nNum == 14 ) {
		job = "?靺攽?";
		level = "51~55";
	} else if ( nNum == 15 ) {
		job ="?靺攽?";
		level = "56~60";
	} else if ( nNum == 16 ) {
		job ="?靺攽?";
		level = "61~65";
	} else if ( nNum == 17 ) {
		job = "?靺攽?";
		level = "66~70";
	}
	else if ( nNum == 18 ) {
		job = "蝶??橫裔";
		level = "51~55";
	} else if ( nNum == 19 ) {
		job = "蝶??橫裔";
		level = "56~60";
	} else if ( nNum == 20 ) {
		job = "蝶??橫裔";
		level = "61~65";
	} else if ( nNum == 21 ) {
		job = "蝶??橫裔";
		level = "66~70";
	}
	else if ( nNum == 22 ) {
		job = "嬪濠萄(碳/絮)";
		level = "51~55";
	} else if ( nNum == 23 ) {
		job = "嬪濠萄(碳/絮)";
		level = "56~60";
	} else if ( nNum == 24 ) {
		job = "嬪濠萄(碳/絮)";
		level = "61~65";
	} else if ( nNum == 25 ) {
		job = "嬪濠萄(碳/絮)";
		level = "66~70";
	}
	else if ( nNum == 26 ) {
		job = "嬪濠萄(橡擠/廓偃)";
		level = "51~55";
	} else if ( nNum == 27 ) {
		job = "嬪濠萄(橡擠/廓偃)";
		level = "56~60";
	} else if ( nNum == 28 ) {
		job = "嬪濠萄(橡擠/廓偃)";
		level = "61~65";
	} else if ( nNum == 29 ) {
		job = "嬪濠萄(橡擠/廓偃)";
		level = "66~70";
	}
	else if ( nNum == 30 ) {
		job = "贗溯葛";
		level = "51~55";
	} else if ( nNum == 31 ) {
		job = "贗溯葛";
		level = "56~60";
	} else if ( nNum == 32 ) {
		job = "贗溯葛";
		level = "61~65";
	} else if ( nNum == 33 ) {
		job = "贗溯葛";
		level = "66~70";
	}
	else if ( nNum == 34 ) {
		job = "?暩?";
		level = "51~55";
	} else if ( nNum == 35 ) {
		job = "?暩?";
		level = "56~60";
	} else if ( nNum == 36 ) {
		job = "?暩?";
		level = "61~65";
	} else if ( nNum == 37 ) {
		job = "?暩?";
		level = "66~70";
	}
	else if ( nNum == 38 ) {
		job = "餌熱";
		level = "51~55";
	} else if ( nNum == 39 ) {
		job = "餌熱";
		level = "56~60";
	} else if ( nNum == 40 ) {
		job = "餌熱";
		level = "61~65";
	} else if ( nNum == 41 ) {
		job = "餌熱";
		level = "66~70";
	}
	else if ( nNum == 42 ) {
		job = "橫諍褐";
		level = "51~55";
	} else if ( nNum == 43 ) {
		job = "橫諍褐";
		level = "56~60";
	} else if ( nNum == 44 ) {
		job = "橫諍褐";
		level = "61~65";
	} else if ( nNum == 45 ) {
		job = "橫諍褐";
		level = "66~70";
	}
	else if ( nNum == 46 ) {
		job = "衛??";
		level = "51~55";
	} else if ( nNum == 47 ) {
		job = "衛??";
		level = "56~60";
	} else if ( nNum == 48 ) {
		job = "衛??";
		level = "61~65";
	} else if ( nNum == 49 ) {
		job = "衛??";
		level = "66~70";
	}
	v0 = self->askYesNo( "霜機擎 #b" .. job.. "#k, 溯漣擎 #b" .. level.. "#k 檣 耀掘蒂 ?飄撚? 霤陛衛麵爾啊橫? 劃陛 謠擎 橾檜 儅望雖紫 路塭.\r\n?狫鷏? 薯寰橾 閨檜棲梱 窕 ?? ?尪銧? 橈橫. 虜擒 斜傖 殮濰?炾? 談棻賊 嬴棲螃蒂 摹鷗?牉? 腋." );
	if ( v0 == 0 ) {
		return 0;
	} else {
		result = premission_decision( nNum );
		if  ( result == 0 ) {
			return 0;
		} else if ( result == 1 ) {
			rNum = random( 0, 3 );
			if ( rNum == 0 ) {
--//				self->say( "諦辦~ 渠欽?捄?? 頂陛 蜓?? 渠煎 詞 蜃鏡橫. #b殮濰?? 陽 30碟除 奢問溘, 葆溘 鼻蝓 葆徹#k擊 勘橫還啪. 罹褐曖 ?蝩?擊 瓊朝等 紫遺檜 腆剪撿." );
				return 1;
			} else if ( rNum == 1 ) {
--//				self->say( "諦辦~ 渠欽?捄?? 頂陛 蜓?? 渠煎 詞 蜃鏡橫. #b殮濰?? 陽 30碟除 僭葬寞橫溘, 葆徹寞橫溘 鼻蝓 葆徹#k擊 勘橫還啪. 罹褐曖 ?蝩?擊 瓊朝等 紫遺檜 腆剪撿." );
				return 2;
			} else if ( rNum == 2 ) {
--//				self->say( "諦辦~ 渠欽?捄?? 頂陛 蜓?? 渠煎 詞 蜃鏡橫. #b殮濰?? 陽 30碟除 檜翕樓紫, 薄?褶? 鼻蝓 葆徹#k擊 勘橫還啪. 罹褐曖 ?蝩?擊 瓊朝等 紫遺檜 腆剪撿." );
				return 3;
			} else if ( rNum == 3 ) {
--//				self->say( "諦辦~ 渠欽?捄?? 頂陛 蜓?? 渠煎 詞 蜃鏡橫. #b殮濰?? 陽 30碟除 貲醞徽, ?裔?徽 鼻蝓 葆徹#k擊 勘橫還啪. 罹褐曖 ?蝩?擊 瓊朝等 紫遺檜 腆剪撿." );
				return 4;
			} 
		} else if ( result == 2 ) {
			self->say( "嘐暮擊 虜褶?狫? 跤?葥鑒?, ?飄撚衋? 賅舒 檜夠縑 氈雖 彊嬴. 霜機擎 #b" .. job.. "#k,  溯漣擎 #b" .. level.. "#k 塭掘." );
			return;
		}
	}
	return -1;
}


--//?ㄟ巡? 葬錶萄
function feellike_reward(  like ) {
	inven = inventory;

	self->say( "謝謝你給我這麼多美味的食物，我想我應該送你一些小禮物來報答你，請確認你的背包還有足夠的欄位！" );

	if ( like >= 100 and like < 200 ) {
		num = random( 1, 100 );
		if  ( num >= 1 and num <= 20 ) {
			ret = inven->exchange( 0, 2020013, 1 );
		} else if ( num >= 21 and num <= 40 ) {
			ret = inven->exchange( 0, 2020014, 1 );
		} else if ( num >= 41 and num <= 42 ) {
			ret = inven->exchange( 0, 2040708, 1 );--//褐檜樓10
		} else if ( num >= 43 and num <= 60 ) {
			ret = inven->exchange( 0, 2020015, 1 ); --//?笮母抻?
		} else if ( num >= 61 and num <= 80 ) {
			ret = inven->exchange( 0, 2000002, 5 );
		} else {
			ret = inven->exchange( 0, 2000002, 10 );
		}
		if ( ret != 0 ) {
			self->say( "很抱歉，你的背包滿了，因此無法獲得獎勵。" );
			return;
		}
	}
	else if ( like >= 200 and like < 300 ) {
		num = random( 1, 50 );
		if  ( num >= 1 and num <= 10 ) {
			ret = inven->exchange( 0, 2020013, 1 ); --//褐煙
		} else if ( num >= 11 and num <= 20 ) {
			ret = inven->exchange( 0, 2020014, 1 );
		} else if ( num == 21  ) {
			ret = inven->exchange( 0, 2040708, 1 ); --// 褐檜樓 10
		} else if ( num == 22 ) {
			ret = inven->exchange( 0, 2040707, 1 ); --// 褐檜樓 60
		} else if ( num >= 23 and num <= 30 ) {
			ret = inven->exchange( 0, 2020015, 1 );
		} else if ( num >= 31 ) {
			ret = inven->exchange( 0, 2000002, 10 );
		}
		if ( ret != 0 ) {
			self->say( "很抱歉，你的背包滿了，因此無法獲得獎勵。" );
			return;
		}
	}
	else if ( like >= 300 and like <= 400 ) {
		num = random( 1, 100 );
		if  ( num >= 1 and num <= 20 ) {
			ret = inven->exchange( 0, 2020013, 1 ); --//褐煙曖 辦嶸
		} else if ( num >= 20 and num <= 40 ) {
			ret = inven->exchange( 0, 2020014, 1 ); --//億漁曖 檜蝸
		} else if ( num >= 21 and num <= 22 ) {
			ret = inven->exchange( 0, 2040707, 1 ); --//褐檜樓 60
		} else if ( num >= 22 and num <= 24 ) {
			ret = inven->exchange( 0, 2040708, 1 ); --//褐檜樓 10
		} else if ( num >= 25 and num <= 50 ) {
			ret = inven->exchange( 0, 2020015, 15 );
		} else {
			ret = inven->exchange( 0, 2000002, 10 );
		}
		if ( ret != 0 ) {
			self->say( "很抱歉，你的背包滿了，因此無法獲得獎勵。" );
			return;
		}
	}
	return;
}

function food_code(  code ) {
	if ( code == 2 ) return 2022001;
	if ( code == 3 ) return 2020004;
	if ( code == 4 ) return 2022003;
	if ( code == 5 ) return 2001001;
	if ( code == 6 ) return 2020028;
	if ( code == 7 ) return 2001002;	
	return 2020001;
}

--//詳擎 擠衝 牖憮縑 評艇剪
function ate_food(  code ) {
	val = qr->get( 7522 );
	food1 = tonumber( substring( val, 3, 1 ));
	food2 = tonumber( substring( val, 4, 1 ));
	food3 = tonumber( substring( val, 5, 1 ));
	food4 = tonumber( substring( val, 6, 1 ));

	val2 = substring( val, 0, 3).. substring( val, 4, 3 ) .. ( code );
	qr->setState( 7522, 1, val2 );

	if ( food1 == code and food2 == code and food3 == code and food4 == code ) {
		self->say( "還是#b#t" .. food_code( code ).. "##k？雖然我很喜歡吃，但已經有點膩了..." );
		return 2;
	} else if ( food2 == code and food3 == code and food4 == code ) {
		self->say( "#b#t" .. food_code( code ).. "##k已經吃了非常多，我想吃點別的東西。" );
		if ( code == 1 ) return 1;
		if ( code == 2 ) return 3;
		if ( code == 3 ) return 3;
		if ( code == 4 ) return 7;
		if ( code == 5 ) return 20;
		if ( code == 6 ) return 26;
		if ( code == 7 ) return 30;
	} else if ( food3 == code and food4 == code ) {
		self->say( "喔～這不是#b#t" .. food_code( code ).. "##k嗎？我超喜歡的！" );
		if ( code == 1 ) return 2;
		if ( code == 2 ) return 4;
		if ( code == 3 ) return 4;
		if ( code == 4 ) return 10;
		if ( code == 5 ) return 27;
		if ( code == 6 ) return 32;
		if ( code == 7 ) return 40;
	} else if ( food4 == code ) {
		self->say( "喔～這不是#b#t" .. food_code( code ).. "##k嗎？我超喜歡的！" );
		if ( code == 1 ) return 2;
		if ( code == 2 ) return 5;
		if ( code == 3 ) return 5;
		if ( code == 4 ) return 13;
		if ( code == 5 ) return 30;
		if ( code == 6 ) return 40;
		if ( code == 7 ) return 50;
	} else if ( food1 != code and food2 != code and food3 != code and food4 != code ) {
		self->say( "哇，這個？#b#t" .. food_code( code ).. "##k？？？我想試試看！" );
		if ( code == 1 ) return 4;
		if ( code == 2 ) return 8;
		if ( code == 3 ) return 8;
		if ( code == 4 ) return 19;
		if ( code == 5 ) return 39;
		if ( code == 6 ) return 50;
		if ( code == 7 ) return 69;
	}	
	else {
		if ( code == 1 ) {
			self->say( "哇！#b炸雞#k！！沒有任何食物有跟它一樣外酥內軟的口感！" );
			return 3;
		} else if ( code == 2 ) {
			self->say( "哇！～ #b#t2022001##k！！！既解渴又可口的美食！我非常喜歡！" );
			return 6;
		} else if ( code == 3 ) {
			self->say( "哇！～ 這不是我的最愛#b漢堡#k嘛！看看它們，看起來非常可口不是嗎？" );
			return 6;
		} else if ( code == 4 ) {
			self->say( "哇！～ #b烤鰻魚#k！！配上美味的甜醬...嗯...非常的棒～" );
			return 16;
		} else if ( code == 5 ) {
			self->say( "哇！～ 在夏天享受#b棒冰棒#k是這輩子最美好的事情了！" );
			return 36;
		} else if ( code == 6 ) {
			self->say( "哇！～ #b巧克力#k可以煩惱跟不愉快一起帶到胃裡，來試試看吧～" );
			return 47;
		} else if ( code == 7 ) {
			self->say( "欸！？這不是美味的#b紅豆刨冰#k嗎！看著上面美味可口的紅豆與水果與雪花冰，我都等不及了。" );
			return 65;
		}
	}
	return 0;
}

--//?ㄟ巡腕雩?
function feellike(  item ) {
	--
	cTime = currentTime();

	if ( qr->get( 7523 ) == "" ) {
		lTime = cTime;
		qr->setState( 7523, 1, lTime );
	} else lTime = qr->get( 7523 );

--//	say = lTime;
--//	target->chatMsg(5, say );

	val2 = qr->get( 7522 );

	if ( val2 == "" ) {
		val2 = "1000000";
		qr->setState( 7522, 1, val2 );
	}
	
	like = tonumber( substring( val2, 0, 3 ) );

	if ( lTime != cTime ) {
		tTime = compareTime( cTime, lTime );
		if ( tTime > 5040 ) {
			if ( like >= 130 ) {
				like = like - 30;
			} else {
				like = 100;
			}
		}
	}

	qr->setState( 7523, 1, cTime );

	if ( item == 1 ) {
		v0 = random( 1, 100 );
		v1 = ate_food( 1 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉吃了炸雞後感覺心情變好了。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉喜歡吃炸雞。" );
			like = like + 1;
		}
	} else if ( item == 2 ) {
		v1 = ate_food( 2 );
		v0 = random( 1, 100 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉喝了小豆粥之後感覺心情變得更加舒暢。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉喜歡小豆粥。" );
			like = like + 1;
		}
	} else if ( item == 3 ) {
		v1 = ate_food( 3 );
		v0 = random( 1, 100 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉吃了漢堡之後感覺心情變得更加舒暢。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉喜歡吃漢堡。" );
			like = like + 1;
		}
	} else if ( item == 4 ) {
		v1 = ate_food( 4 );
		v0 = random( 1, 100 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉吃了烤鰻魚之後感覺心情變得更加舒暢。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉喜歡吃烤鰻魚。" );
			like = like + 2;
		}
	} else if ( item == 5 ) {
		v1 = ate_food( 5 );
		v0 = random( 1, 100 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉享受了棒冰棒後，覺得自己變得更加愉快。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉非常喜歡棒冰棒。" );
			like = like + 2;
		}
	} else if ( item == 6 ) {
		v1 = ate_food( 6 );
		v0 = random( 1, 100 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉吃了巧克力之後心情變好了。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉喜歡巧克力。" );
			like = like + 2;
		}
	} else if ( item == 7 ) {
		v1 = ate_food( 7 );
		v0 = random( 1, 100 );
		if ( v0 >= 1 and v0 <= v1 ) {
			target->chatMsg(5, "溫莉吃了紅豆刨冰之後心情變得非常愉悅。" );
			if ( random( 1, 20 ) == 1 ) {
				feellike_reward( like );
			}
			like = like + 5;
		} else {
			target->chatMsg(5, "溫莉喜歡吃紅豆刨冰。" );
			like = like + 3;
		}
	} 
	else if ( item == -1 ) {
		if ( like >= 103 ) {
			target->chatMsg(5, "溫莉的心情變差了。" );
			like = like - 3 ;
		} else {
			target->chatMsg(5, "溫莉的心情變差了。" );
			like = 100;
		}
	}

	if ( like > 500 ) {
		like = 500;
	} else if ( like < 100 ) {
		like = 100;
	}

	if ( item != -1 ) {
		food = substring( val2, 4, 3) .. ( item );
		result = ( like ) .. food;
		qr->setState( 7522, 1, result );
	} else {
		food = substring( val2, 3, 4 );
		result = ( like ) .. food;
		qr->setState( 7522, 1, result );
	}

--//	say = qr->get( 7522 );
--//	target->chatMsg(5, say );
}

--//贗葬橫嘐暮 頂辨
function clearmission2(  nNum ) {
	quest = FieldSet.get("Party3");
	if ( nNum == 1 ) {
		v0 = self->askYesNo( "Voc?pode me fazer um favor? Tente completar a #bMiss緌 do Grupo<Tra蔞s da Deusa>#k em menos de 25 minutos. Estou faminto, sabe. Se puder fazer isto, eu vou dar a voc?algum EXP extra." );
		if( v0 == 0 ) {
			self->say( "AH, entendo. Azar meu, mas entendo. Pe蔞 uma outra hora~" );
			quest->setVar( "cMission", "0" );			
		} else {
			quest->setVar( "cMission", "1" );
			say = "Voc?precisa terminar <Miss緌> Miss緌 do Grupo <Tra蔞s da Deusa> em menos de 25 minutos!";
			quest->broadcastMsg( 6, say );
			self->say( "Certo! O tempo est?passando enquanto falamos. Complete a miss緌!" );
		} 
	} else if ( nNum == 2 ) {
		v0 = self->askYesNo( "Voc?pode me fazer um favor? Fa蓷 a #bMiss緌 do Grupo<Tra蔞s da Deusa>#k e consiga para mim 8 #bSementes Estranhas#k?\r\nAinda n緌 encontrei uma comida que nunca tenha experimentado, mas realmente n緌 fa蔞 id嶯a do gosto destas #bSementes Estranhas#k ~ Quero muito provar isso. E lembre-se de que ?o l獮er do grupo quem deve me entregar." );
		if ( v0 == 0 ) {
			self->say( "AH, entendo. Azar meu, mas entendo. Pe蔞 uma outra hora~" );
			quest->setVar( "cMission", "0" );			
		} else {
			quest->setVar( "cMission", "2" );
			say = "<Miss緌> Junte pelo menos 8 Sementes Estranhas!";
			quest->broadcastMsg( 6, say );
			self->say( "Certo, a?est緌 as 8 #bSementes Estranhas#k~ Certifique-se de que o l獮er do grupo entregue todas!" );
		}
	} else if ( nNum == 3 ) {
		v0 = self->askYesNo( "Voc?pode me fazer um favor? Fa蓷 a #bMiss緌 do Grupo<Tra蔞s da Deusa>#k e consiga para mim os #bLP's#k? Eu preciso de apenas 6. E lembre-se de que ?o l獮er do grupo quem deve me entregar." );
		if ( v0 == 0 ) {
			self->say( "AH, entendo. Azar meu, mas entendo. Pe蔞 uma outra hora~" );
			quest->setVar( "cMission", "0" );			
		} else {
			quest->setVar( "cMission", "3" );
			say = "<Miss緌> Junte 6 LP?s de m?sicas variadas!";
			quest->broadcastMsg( 6, say );
			self->say( "Certo, aqui est緌 os 6 #bLP's#k~ Eu n緌 ligo para o tipo, mas o l獮er do grupo precisa me entregar todos!" );
		}
	} else if ( nNum == 4 ) {
		v0 = self->askYesNo( "Voc?pode me fazer um favor? Voc?poderia eliminar #bLuinels#k na Torre da Deusa?" );
		if ( v0 == 0 ) {
			self->say( "AH, entendo. Azar meu, mas entendo. Pe蔞 uma outra hora~" );
			quest->setVar( "cMission", "0" );			
		} else {
			quest->setVar( "cMission", "4" );
			say = "<Miss緌> Elimine os Luinels!";
			quest->broadcastMsg( 6, say );
			self->say( "Certo! Voc?precisa eliminar CADA UM DELES!" );
		}
	} else if ( nNum == 5 ) {
		v0 = self->askYesNo( "Voc?pode me fazer um favor? Voc?poderia eliminar #bCellion, Lioner Jr. e Grupin Jr.#k na Torre da Deusa?" );
		if ( v0 == 0 ) {
			self->say( "AH, entendo. Azar meu, mas entendo. Pe蔞 uma outra hora~" );
			quest->setVar( "cMission", "0" );			
		} else {
			quest->setVar( "cMission", "5" );
			say = "<Miss緌> Por favor, elimine Cellion Jr., Lioner Jr. e Grupin Jr!";
			quest->broadcastMsg( 6, say );
			self->say( "Certo! Voc?precisa eliminar CADA UM DELES!" );
		}
	} 
	return;
}

--//贗葬橫嘐暮 葬錶萄
function cMission_reward(  code ) {
	quest = FieldSet.get("Party3");

	if ( code == 1 ) {
		cleartime = quest->getQuestTime();
--//		say = " dd " .. string ( cleartime );
--//		target->chatMsg(5, say );
		if ( cleartime < 1504 ) {
			say = "<Miss緌 Completa> EXP adicional como recompensa por completar a miss緌 dentro de 25 minutos.";
			quest->broadcastMsg( 6, say );
			quest->incExpAll( 10000 );
		}
	} else if ( code == 2 ) {
		inven = inventory;
		nItem = inven->itemCount( 4001053 );
		if (  nItem >= 8 ) {
--//			ret = inven->exchange( 0, 4001053, -nItem );
			say = "<Miss緌 Completa> EXP adicional como recompensa por coletar as 8 Sementes Estranhas com sucesso.";
			quest->broadcastMsg( 6, say );
			quest->incExpAll( 5000 );
		}
	} else if ( code == 3 ) {
		inven = inventory;
		nItem = inven->itemCount( 4001056 ) + inven->itemCount( 4001057 ) + inven->itemCount( 4001058 )+ inven->itemCount( 4001059 ) + inven->itemCount( 4001060 ) +  inven->itemCount( 4001061 ) + inven->itemCount( 4001062 );
		if ( nItem >= 6 ) {
			say = "<Miss緌 Completa> EXP adicional como recompensa por coletar os 6 LP?s.";
			quest->broadcastMsg( 6, say );
			quest->incExpAll( 6000 );
		}
	} else if ( code == 4 ) {
		mNum =  Field.get( 920010910 ).getMobCount( 9300044 ) + Field.get( 920010920 ).getMobCount( 9300044 ) + Field.get( 920010930 ).getMobCount( 9300044 );
		if ( mNum == 0 ) {
			say = "<Miss緌 Completa> EXP adicional como recompensa por eliminar Luinel com sucesso.";
			quest->broadcastMsg( 6, say );
			quest->incExpAll( 8000 );
		}
	} else if ( code == 5 ) {
		mNum =  Field.get( 920011000 ).getMobCount( 9300051 ) + Field.get( 920011000 ).getMobCount( 9300052 ) + Field.get( 920011000 ).getMobCount( 9300053 );
		if ( mNum == 0 ) {
			say = "<Miss緌 Completa> EXP adicional como recompensa por eliminar Cellion Jr., Grupon Jr. e Lioner Jr. com sucesso!";
			quest->broadcastMsg( 6, say );
			quest->incExpAll( 5000 );
		}
	}
	return;
}

--//贗葬橫 嘐暮 嫦翕
function clearmission() {
	--
	quest = FieldSet.get("Party3");
	val = qr->get( 7522 );
	if ( quest->getVar( "cMission" ) == "" ) {
		like = tonumber( substring( val, 0, 3 ) );
		v0 = random( 1, 15 );
		if ( like >= 100 and like < 200 ) {
			if ( v0 >= 1 and v0 <= 3 ) {
				v1 = random( 1, 5 );
				clearmission2( v1 );
			}
		}
		else if ( like >= 200 and like < 300 ) {
			if ( v0 >= 1 and v0 <= 5 ) {
				v1 = random( 1, 5 );
				clearmission2( v1 );
			}
		}
		else if ( like >= 300 and like <= 400 ) {
			if ( v0 >= 1 and v0 <= 8 ) {
				v1 = random( 1, 5 );
				clearmission2( v1 );
			}
		}
	}
	return;
}

--//嶽酈
function s_party3_enter() {
	field = self->getField();
	--
	val = qr->get( 7521 );
	val2 = qr->get( 7522 );
	--val = val2;

	if ( val2 == "" ) {
		val2 = "1000000";
		val3 = currentTime();
		qr->setState( 7522, 1, val2 );
		qr->setState( 7523, 1, val3 );
	}

	--//殮濰裘
	if ( field->getID() == 200080101 ) {
		loop_cond = 1;
		repeat{
		--while (loop_cond == 1) {
			
			
--//			v0 = self->askMenu( "寰喟? 陪 蹂薑 錄酈塭堅 ??. 罹褐曖 鱉擊 賅?駍炾? 談戲賊 釭縑啪 檜撿晦 ?炩饇瞶 \r\n#b#L0# 殮濰擊 褐羶?挬?.#l\r\n#L1# 罹褐曖 鱉縑 渠?? 僖朝棻.\r\n#L2# 錄酈縑啪 詳擊 匙擊 遽棻.#l" );

			if ( tonumber( substring( val2, 0, 3)) >= 100 and tonumber( substring( val2, 0, 3 ) ) < 200 ) {
			v0 = self->askMenu( "你好？我是溫莉。如果想要到雅典娜禁地冒險就儘管告訴我～對了，如果隊伍中有劍士、法師、盜賊、弓箭手、海盜各一人以上，我就送你溫莉的祝福。\r\n#b#L0# 申請入場。#l\r\n#L1# 針對雅典娜禁地詢問。\r\n#L2# 給溫莉吃的東西。#l" );
			} else if ( tonumber( substring( val2, 0, 3)) >= 200 and tonumber( substring( val2, 0, 3 ) ) < 300 ) {
				v0 = self->askMenu( "嗨！非常高興見到你！我叫做溫莉。如果想要到雅典娜禁地冒險就儘管告訴我～對了，如果隊伍中有劍士、法師、盜賊、弓箭手、海盜各一人以上，我就送你溫莉的祝福。\r\n#b#L0# 申請入場。#l\r\n#L1# 針對雅典娜禁地詢問。\r\n#L2# 給溫莉吃的東西。#l" );
			} else if ( tonumber( substring( val2, 0, 3 ) ) >= 300 and tonumber( substring( val2, 0, 3 ) ) <= 400) {
				v0 = self->askMenu( "嗨！#b" .. target->getName().. "#k。想要到雅典娜禁地冒險就儘管告訴我～對了，如果隊伍中有劍士、法師、盜賊、弓箭手、海盜各一人以上，我就送你溫莉的祝福。\r\n#b#L0# 申請入場。#l\r\n#L1# 針對雅典娜禁地詢問。\r\n#L2# 給溫莉吃的東西。#l" );
			} else {
				v0 = self->askMenu( "嗨，我是溫莉，如果想要到雅典娜禁地冒險，請告訴我...#l\r\n#L1# 針對雅典娜禁地詢問。\r\n#L2# 給溫莉吃的東西。#l" );
			}
			if ( v0 == 0 ) {
				if ( target->isPartyBoss() != 1 ) {
					self->say( "嘿，請隊長與我交談！" );
					return;
				}
				
				mis = premission_prev();

--// blocked by jk 20080730


				mis = 0;
				cTime = currentTime();
				if ( val != "" ) {
	
					--//衛除檜 夥船橫 億煎遴 嘐暮 嫡擠
					eTime = compareTime( cTime ,substring( val, 2, 14 ) );
					if ( eTime >= 60 ) {
						rNum = random( 10, 95 );
						result =  rNum  .. cTime;
--//						target->chatMsg(5, result );
						qr->setState( 7521, 1, result  );
						if ( rNum >= 10 and rNum <= 49 ) {
							mis = premission( rNum );
						} 
					--//晦襄 嘐暮 營撲貲 塽 ??薑
					} else {
						rNum = tonumber( substring( qr->get( 7521 ), 0, 2 ));
--//						k = qr->get( 7521 );
--//						target->chatMsg(5, k );
						if ( rNum >= 10 and rNum <= 49 ) {
							mis = premission( rNum );
						}
					}
				}
				--//蟾晦??
				else {
					rNum = random( 10, 95 );
					result = ( rNum ) .. cTime;
--//					target->chatMsg(5, result );
					qr->setState( 7521,  1, result );
					if ( rNum >= 10 and rNum <= 49 ) {
						mis = premission( rNum );
					} 
				}
				setParty = FieldSet.get( "Party3" );
				res = setParty->enter( userID, 0 );
				if ( res == -1 ) self->say( "Tenho motivos para n緌 deixar voc?entrar. Tente de novo mais tarde." );
				else if ( res == 1 ) self->say( "Voc?n緌 est?no grupo. Voc?precisa estar em um grupo para fazer isto!" );
				else if ( res == 2 ) self->say( "Seu grupo n緌 possui 6 membros! Preciso de 6!!" );
				else if ( res == 3 ) self->say( "Algu幦 no seu grupo n緌 est?entre N癉eis 51 ~70. Deixe seu grupo dentro do n癉el!" );
				else if ( res == 4 ) self->say( "Um outro grupo j?entrou para completar a miss緌. Tente de novo mais tarde." );
				else {
					if ( mis == 1 ) {
						target->givePartyBuff( 2022090 );
						say = "滿足了溫莉的需求，因此獲得了30分鐘的攻擊力與魔法攻擊力提升效果。";
						setParty->broadcastMsg( 6, say );
					} else if ( mis == 2 ) {
						target->givePartyBuff( 2022091 );
						say = "滿足了溫莉的需求，因此獲得了30分鐘的物理防禦力與魔法防禦力提升效果。";
						setParty->broadcastMsg( 6, say );
					} else if ( mis == 3 ) {
						target->givePartyBuff( 2022092 );
						say = "滿足了溫莉的需求，因此獲得了30分鐘的命中率與迴避率上升效果。";
						setParty->broadcastMsg( 6, say );
					} else if ( mis == 4 ) {
						target->givePartyBuff( 2022093 );
						say = "滿足了溫莉的需求，因此獲得了30分鐘的移動速度與跳躍力提升效果。";
						setParty->broadcastMsg( 6, say );
					}
					
					rand_a1 = random( 1, 10 );
					rand_a2 = random( 1, 10 );
					rand_a3 = random( 1, 10 );

					if ( rand_a1 == 1 or rand_a1 == 2 ) {
						setParty->setReactorState( 16, "party3_box01", 1, 0 );
					}
					if ( rand_a2 == 3 or rand_a2 == 4 ) {
						setParty->setReactorState( 19, "party3_box02", 1, 0 );
					}
					if ( rand_a3 == 5 or rand_a3 == 6 ) {
						setParty->setReactorState( 22, "party3_box03", 1, 0 );
					}
					--setParty->setReactorState( 4, "music", dayOfWeek(), 0);
					party3_takeawayitem();
				}
				return;
			} else if ( v0 == 1 ) {
				self->say( "H?alguns dias atr嫳, depois de uma grande tempestade, um novo caminho nublado surgiu no alto da Torre de Orbis, atr嫳 da #best嫢ua da Deusa Minerva#k. As nuvens se dissiparam e uma nova e misteriosa torre surgiu." ); 
				self->say( "Conclu璥os que a torre que estamos procurando ?aquela de um ser que fica por tr嫳, controlando Orbis. #bDeusa Minerva#k. Tenho toda a certeza de que a #bDeusa Minerva#k est?presa em algum lugar da torre. O que voc?acha? Voc?gostaria de explorar a torre e encontrar os restos da Deusa?" );
			} else if ( v0 == 2 ) {
				inven = inventory;
				v1 = self->askMenu( "嘿，你有什麼吃的嗎？！\r\n#b#L0# #t2020001##l\r\n#b#L1# #t2022001##l\r\n#b#L2# #t2020004##l\r\n#b#L3# #t2022003##l\r\n#b#L4# #t2001001##l\r\n#b#L5# #t2020028##l\r\n#b#L6# #t2001002##l" );
				if ( v1 == 0 ) {
					nItem = inven->itemCount( 2020001 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2020001, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2020001#。" );
							return;
						}
						feellike( 1 );
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2020001#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				} else if ( v1 == 1 ) {
					nItem = inven->itemCount( 2022001 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2022001, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2022001#。" );
							return;
						}
						feellike( 2 );
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2022001#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				}
				else if ( v1 == 2 ) {
					nItem = inven->itemCount( 2020004 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2020004, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2020004#。" );
							return;
						}
						feellike( 3 );			
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2020004#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				}
				else if ( v1 == 3 ) {
					nItem = inven->itemCount( 2022003 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2022003, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2022003#。" );
							return;
						}
						feellike( 4 );
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2022003#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				}
				else if ( v1 == 4 ) {
					nItem = inven->itemCount( 2001001 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2001001, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2001001#。" );
							return;
						}
						feellike( 5 );
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2022001#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				}
				else if ( v1 == 5 ) {
					nItem = inven->itemCount( 2020028 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2020028, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2020028#。" );
							return;
						}
						feellike( 6 );
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2020028#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				}
				else if ( v1 == 6 ) {
					nItem = inven->itemCount( 2001002 );
					if ( nItem > 0 ) {
						ret = inven->exchange( 0, 2001002, -1 );
						if ( ret == 0 ) target->getField()->effectSound( "Grupo3/Comer" );
						else {
							target->chatMsg(5, "沒辦法給他吃。沒足夠的#t2001002#。" );
							return;
						}
						feellike( 7 );
						return;
					} else {
						feellike( -1 );
						self->say( "什麼？你說要給我吃#t2001002#，可是卻沒帶來，你想惹我生氣嗎？" );
						return;
					}
				}
			}
		} until(true)
	} else if ( field->getID() == 920010000) {
		quest = FieldSet.get("Party3");
		if ( target->isPartyBoss() == 1 ) {
			if ( quest->getVar( "cm" ) != "1" ) {
				quest->setVar( "cm", "1" );
				clearmission();
			} 
			v0 = self->askYesNo( "你確定要離開這離嗎？一旦離開就必須重新來過..." );
		}
		if ( v0 == 0 ) self->say( "請你與隊伍成員們繼續加油。" );
		else {
			self->say( "再見了～" );
			target->transferField( 920011200, "st00" );
		}
	}

--//blocked by jk 20080730
}



function party3_help() {
	field = self->getField();
	if ( field->getID() == 920010000 ) self->say( "N緌 sei o que dizer. Obrigado por juntar meu corpo. Eu sou Eak, o camareiro de confian蓷 da Deusa Minerva. Voc瘰 est緌 aqui para resgatar Minerva, n緌 ? Eu vou tentar ajudar. Certo, vou levar voc瘰 at?a entrada da torre." );
	else if ( field->getID() == 920010100 ) {
		self->say( "Sim, a est嫢ua quebrada ao meu lado ?onde #bMinerva#k est?presa no momento. Para resgatar #bMinerva#k, voc?vai precisar juntar #b6 peda蔞s da est嫢ua #k para consertar a parte quebrada e trazer o #b#t4001055##k para aplicar o poder m疄tico na est嫢ua." );
		self->say( "Procure pela torre com cuidado para encontrar o #bPeda蔞 da Est嫢ua da Deusa#k e a #bErva da Vida#k para resgatar a Deusa Minerva!" );
	}
	else if ( field->getID() == 920010200 ) self->say( "Esta ?a alameda para a Torre da Deusa. Os duendes quebraram #b#t4001044# em 30 peda蔞s#k e levaram cada um deles. Por favor, acabe com os Duendes e traga de volta o #b#t4001050##k; em troca, eu vou fazer #b#t4001044##k com eles. Os Duendes se fortaleceram com o poder da est嫢ua da Deusa, por isso, tenha cuidado~" );
	else if ( field->getID() == 920010300 ) self->say( "Este era o antigo dep鏀ito da Torre da Deusa, mas agora virou o lar dos Cellions. Um Cellion pegou #b#t4001045##k e escondeu. Voc?deve derrot?lo e trazer #b#t4001045##k de volta." );
	else if ( field->getID() == 920010400 ) {
		self->say( "Esta ?a sala da Torre da Deusa. ?aqui que a Deusa Minerva gosta de ouvir m?sica. Ela adorava ouvir v嫫ios tipos de m?sica, dependendo do dia da semana." );
		self->say( "A Deusa gostava de ouvir diferentes tipos de m?sica, dependendo do dia.\r\n #bNa segunda-feira, ela gosta de m?sica bonitinha; algo encantador e ador嫛el\r\nNa ter蓷, m?sicas assustadoras, que, meu Deus, tiravam qualquer um do s廨io\r\nNa quarta, m?sica divertida, algo para dan蓷r;\r\nNa quinta, m?sica triste;\r\nNa sexta, ela gosta de m?sica que d?frio na espinha\r\nNo s墎ado, ela preferia ouvir m?sica r嫚ida, com arranjos curtos\r\nE, no domingo, 鏕era para combinar com o humor#k\r\na Deusa mudava de gosto todo dia. Ela estudava de verdade a m?sica." );
		self->say( "Se voc?tocar a m?sica de antes, o esp甏ito da Deusa Minerva pode reagir e... alguma coisa pode acontecer." );
	}
	else if ( field->getID() == 920010500 ) {
		self->say( "Esta ?a Sala lacrada da Torre da Deusa. ?a sala onde a Deusa Minerva se sentia segura o bastante para manter suas posses valiosas." );
		self->say( "Os tr瘰 degraus ao lado funcionam como travas que podem liberar a maldi誽o e todos eles precisam sustentar o mesmo peso. Vamos ver... isso vai exigir que cinco de voc瘰 fiquem em cima para chegar ao peso ideal. Al幦 disso, voc瘰 v緌 precisar resolver o problema em at?7 tentativas e n緌 mudar a resposta, ou ser緌 banidos da sala lacrada." );
	}
	else if ( field->getID() == 920010600 ) self->say( "Esta ?o sagu緌 da Torre da Deusa, onde os convidados ficam por algumas noites. #b#t4001048##k se quebrou em 40 peda蔞s espalhados por todo o local. Por favor, encontre e junte os peda蔞s de #b#t4001052##k." );
	else if ( field->getID() == 920010700 ) self->say( "Este ?o caminho para o alto da Torre da Deusa. No alto, existem 5 alavancas que controlam a porta para o cume. Seu dever ?identificar as duas alavancas que precisam ser movidas corretamente. Feito isso, avise-me para que eu possa dizer se as alavancas corretas foram movidas ou n緌." );
	else if ( field->getID() == 920010800 ) self->say( "Este ?o jardim da Torre da Deusa. A #bErva da Vida#k s?pode crescer neste vaso de flores. Basta cultivar a erva para obter a semente... ah, falando nisso, tenha cuidado. Voc瘰 podem se encontrar com #bDuende-Papai#k, o pr鏕rio que prendeu a Deusa na est嫢ua." );
	else if ( field->getID() == 920010900 ) self->say( "Esta ?a Sala da Culpa da Torre da Deusa. A Deusa Minerva costumava manter presos neste lugar os monstros mais sinistros e que cometiam os piores crimes. Felizmente, nenhuma parte da est嫢ua se encontra neste local, por isso, voltem pela ladeira ?direita desta sala. Digo mais uma vez... algo pode estar escondido..." );
	else if ( field->getID() == 920011000 ) self->say( "Argh, est?t緌 escuro aqui. Voc?agora se encontra na Sala da Escurid緌 da Torre da Deusa. Ei, como voc瘰 chegaram aqui? Voc?n緌 vai encontrar nenhuma parte da est嫢ua da deusa aqui. Sugiro que voc?procure nos outros quartos." );

	if ( target->isPartyBoss() != 1 ) self->say( "Agora continue, por favor, com o l獮er do seu grupo mostrando o caminho." );
	return;
}

--//黴濰 蝶觼董??
function party3_out() {
	field = self->getField();

	if ( field->getID() != 920011200 ) { 
		v0 = self->askMenu( "O que vai fazer? \r\n#b#L0#Obedecer a Eak.#l\r\n#b#L1# Abandonar a Miss緌 do Grupo e ir embora.#l" );
		if ( v0 == 0 ) {
			party3_help();
			return;
		} else if ( v0 == 1 ) {
			v1 = self->askYesNo( "Quer realmente sair?" );
			if ( v1 == 0 ) self->say( "Pense bem nas suas op踥es e converse comigo." );
			else target->transferField( 920011200, "st00" );
		}
	} else {
		--//self->say( "Indo embora..." );
		--//嬴檜蠱 趙晦
		party3_takeawayitem();
		--//幗?謐善?
		target->cancelPartyBuff( 2022090 );
		target->cancelPartyBuff( 2022091 );
		target->cancelPartyBuff( 2022092 );
		target->cancelPartyBuff( 2022093 );
		target->transferField( 200080101, "st00" );
	}
	return;
}

function s_party3_minerva() {
	quest = FieldSet.get("Party3");
	field = self->getField();
	
	if ( field->getID() == 920010100 ) {
		self->say( "謝謝你們修復了雕像幫助我脫離陷阱，願女神的祝福與你們同在..." );
		if ( target->isPartyBoss() != 1 ) {
			self->say( "請隊伍的隊長與我談話..." );
		} else {
			quest->incExpAll( 23000, 7020 );

			--//寞?陊笑偽磈餗?
		--[[/*	cTime = currentTime();
			if ( serverType == 2 ) {
				wsTime = compareTime( cTime, "07/01/09/00/00" );
				weTime =  compareTime( "07/02/04/23/59", cTime );
			} else {
				wsTime = compareTime( cTime, "07/01/16/00/00" );
				weTime =  compareTime( "07/02/04/23/59", cTime );
			}

			if ( wsTime >= 0 and weTime >= 0 ) {
				say = "<Evento Miss緌 de Grupo 2007> Recompensa de EXP extra por completar a Miss緌 do Grupo.";
				quest->broadcastMsg( 6, say );
				quest->incExpAll( 22200 );
			}*/]]--
		        --//嘐暮唳?餗?
			if ( quest->getVar( "cMission" ) != "" and quest->getVar( "cMission" ) != "0"  ) {
				v0 = tonumber( quest->getVar( "cMission" ));
				cMission_reward( v0 );
			}

			--bonusmap = FieldSet( "Party6" );
	        --res = bonusmap.enter( target->getPartyID(), 0 );
	        quest->transferAll( 920011100, "st00" );
		return;
		}
	} else if ( field->getID() == 920011300 ) {
		inven = inventory;

		--//橾晦濰 婦溼 籀葬
		for i = 0,  9  {
			code = 4001064 + i;
			nItem = inven->itemCount( code );
			if ( nItem == 0 ) {
				diary = 0;
				break;
			} else {
				diary = 1;
			}
		}

		if ( diary == 1 ) {
			self->say( "Ei, esta n緌 ?uma p墔ina solta do meu di嫫io? Muito obrigado por encontr?la! Quero dizer, escrevi isto e acabei presa na est嫢ua... h?algumas centenas de anos. Mal consigo lembrar." );
			v0 = self->askYesNo( "O di嫫io diz como acabei presa na est嫢ua. Como sempre digo, n緌 cometo o mesmo erro duas vezes. Vou fazer um registro disto e entregar a voc?" );
			if ( v0 == 0 ) {
				self->say( "Se foi feito um registro, muita coisa pode ser descoberta... hum..." );
			} else {
				nItem = inven->itemCount( 4161014 );
				if ( nItem > 0 ) {
					self->say( "Voc?j?tem #b#t4161014##k." );
				} else {
					ret = inven->exchange( 0, 4001064, -1, 4001065, -1, 4001066, -1, 4001067, -1, 4001068, -1, 4001069, -1, 4001070, -1, 4001071, -1, 4001072, -1, 4001073, -1, 4161014, 1 );
					if ( ret == 0 ) self->say( "Por favor, cuide bem do meu di嫫io." );
					else {
						self->say( "Por acaso voc?.. perdeu aquela p墔ina do meu di嫫io?" );
						return;
					} 
				}
			}	
			

				--[[/*if ( v0 == 0 ) {
				rNum = random( 0, 13 );
				if ( rNum == 0 ) itemid = 2043001;
				else if ( rNum == 1 ) itemid = 2043101;
				else if ( rNum == 2 ) itemid = 2043201;
				else if ( rNum == 3 ) itemid = 2043301;
				else if ( rNum == 4 ) itemid = 2043701;
				else if ( rNum == 5 ) itemid = 2043801;
				else if ( rNum == 6 ) itemid = 2044001;
				else if ( rNum == 7 ) itemid = 2044101;
				else if ( rNum == 8 ) itemid = 2044201;
				else if ( rNum == 9 ) itemid = 2044301;
				else if ( rNum == 10 ) itemid = 2044401;
				else if ( rNum == 11 ) itemid = 2044501;
				else if ( rNum == 12 ) itemid = 2044601;
				else itemid = 2044701;
				ret = inven->exchange( 0, 4001064, -1, 4001065, -1, 4001066, -1, 4001067, -1, 4001068, -1, 4001069, -1, 4001070, -1, 4001071, -1, 4001072, -1, 4001073, -1, itemid, 1 );
				if ( ret != 0 ) {
					self->say( "橾晦濰擊 瓊嬴輿衛堅, ?埏庢? 檗橫爾雖紫 彊戲樟棻棲 薑蜓 耀瞰?牮籀袗?. 薑蜓 馬餌?桭炴?." );
					return;
				} else self->say( "模綠璽縑 綴 蘊檜 氈朝雖 ?挫恉? 輿撮蹂." );
			} else if ( v0 == 1 ) {
				rNum = random( 0, 16 );
				if ( rNum == 0 ) itemid = 2041001;
				else if ( rNum == 1 ) itemid = 2041004;
				else if ( rNum == 2 ) itemid = 2041004;
				else if ( rNum == 3 ) itemid = 2041007;
				else if ( rNum == 4 ) itemid = 2041010;
				else if ( rNum == 5 ) itemid = 2041013;
				else if ( rNum == 6 ) itemid = 2041016;
				else if ( rNum == 7 ) itemid = 2041019;
				else if ( rNum == 8 ) itemid = 2041022;
				else if ( rNum == 9 ) itemid = 2041002;
				else if ( rNum == 10 ) itemid = 2041005;
				else if ( rNum == 11 ) itemid = 2041010;
				else if ( rNum == 12 ) itemid = 2041014;
				else if ( rNum == 13 ) itemid = 2041017;
				else if ( rNum == 14 ) itemid = 2041020;
				else if ( rNum == 15 ) itemid = 2041023;
				else itemid = 2044701;
				ret = inven->exchange( 0, 4001064, -1, 4001065, -1, 4001066, -1, 4001067, -1, 4001068, -1, 4001069, -1, 4001070, -1, 4001071, -1, 4001072, -1, 4001073, -1, itemid, 1 );
				if ( ret != 0 ) {
					self->say( "橾晦濰擊 瓊嬴輿褐勒 堅蛾雖虜, 橫飩啪 陴曖 橾晦濰擊 ?埏庢? 檗橫爾褒熱 氈釭蹂? ??! 薑蜓 褒蜂檜縑蹂." );
					return;
				} else self->say( "模綠璽縑 綴 蘊檜 氈朝雖 ?挫恉? 輿撮蹂." );
			}*/--]]
		}

		--//贗葬橫 爾鼻
		self->say( "非常謝謝你們拯救了我。我有些小禮物想作為對你們英勇行為的回報，請你們務必收下。在接受之前，請確保你的背包裡還有足夠的欄位。" );
		if ( inven->slotCount( 2 ) > inven->holdCount( 2 ) and inven->slotCount( 4 ) > inven->holdCount( 4 ) ) {
			rnum = random( 0, 250 );
			nNewItemID = 0;
			nNewItemNum = 0;
			--//僭擒盟
			if ( rnum == 0 ) { nNewItemID = 2000004; nNewItemNum = 10; }
			else if ( rnum == 1 ) { nNewItemID = 2000002; nNewItemNum = 100; }
			else if ( rnum == 2 ) { nNewItemID = 2000003; nNewItemNum = 100; }
			else if ( rnum == 3 ) { nNewItemID = 2000006; nNewItemNum = 50; }
			else if ( rnum == 4 ) { nNewItemID = 2022000; nNewItemNum = 50; }
			else if ( rnum == 5 ) { nNewItemID = 2022003; nNewItemNum = 50; }
			--//10% 輿僥憮盟
			else if ( rnum == 6 ) { nNewItemID = 2040002; nNewItemNum = 1; }
			else if ( rnum == 7 ) { nNewItemID = 2040402; nNewItemNum = 1; }
			else if ( rnum == 8 ) { nNewItemID = 2040502; nNewItemNum = 1; }
			else if ( rnum == 9 ) { nNewItemID = 2040505; nNewItemNum = 1; }
			else if ( rnum == 10 ) { nNewItemID = 2040602; nNewItemNum = 1; }
			else if ( rnum == 11 ) { nNewItemID = 2040802; nNewItemNum = 1; }
			--//錳戮盟
			else if ( rnum == 12 ) { nNewItemID = 4003000; nNewItemNum = 70; }
			else if ( rnum == 13 ) { nNewItemID = 4010000; nNewItemNum = 20; }
			else if ( rnum == 14 ) { nNewItemID = 4010001; nNewItemNum = 20; }
			else if ( rnum == 15 ) { nNewItemID = 4010002; nNewItemNum = 20; }
			else if ( rnum == 16 ) { nNewItemID = 4010003; nNewItemNum = 20; }
			else if ( rnum == 17 ) { nNewItemID = 4010004; nNewItemNum = 20; }
			else if ( rnum == 18 ) { nNewItemID = 4010005; nNewItemNum = 20; }
			else if ( rnum == 19 ) { nNewItemID = 4010006; nNewItemNum = 15; }
			else if ( rnum == 20 ) { nNewItemID = 4020000; nNewItemNum = 20; }
			else if ( rnum == 21 ) { nNewItemID = 4020001; nNewItemNum = 20; }
			else if ( rnum == 22 ) { nNewItemID = 4020002; nNewItemNum = 20; }
			else if ( rnum == 23 ) { nNewItemID = 4020003; nNewItemNum = 20; }
			else if ( rnum == 24 ) { nNewItemID = 4020004; nNewItemNum = 20; }
			else if ( rnum == 25 ) { nNewItemID = 4020005; nNewItemNum = 20; }
			else if ( rnum == 26 ) { nNewItemID = 4020006; nNewItemNum = 20; }
			else if ( rnum == 27 ) { nNewItemID = 4020007; nNewItemNum = 10; }
			else if ( rnum == 28 ) { nNewItemID = 4020008; nNewItemNum = 10; }
			--//敝堅葬盟
			else if ( rnum == 29 ) { nNewItemID = 1032013; nNewItemNum = 1; }
			else if ( rnum == 30 ) { nNewItemID = 1032011; nNewItemNum = 1; }
			else if ( rnum == 31 ) { nNewItemID = 1032014; nNewItemNum = 1; }
			--//陛檜嬴曖 蜂饜盟
			else if ( rnum == 32 ) { nNewItemID = 1102021; nNewItemNum = 1; }
			else if ( rnum == 33 ) { nNewItemID = 1102022; nNewItemNum = 1; }
			else if ( rnum == 34 ) { nNewItemID = 1102023; nNewItemNum = 1; }
			else if ( rnum == 35 ) { nNewItemID = 1102024; nNewItemNum = 1; }
			--//輿僥憮盟
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
			--//僭擒
			else if ( rnum == 52 ) { nNewItemID = 2000004; nNewItemNum = 35; }
			else if ( rnum == 53 ) { nNewItemID = 2000002; nNewItemNum = 80; }
			else if ( rnum == 54 ) { nNewItemID = 2000003; nNewItemNum = 80; }
			else if ( rnum == 55 ) { nNewItemID = 2000006; nNewItemNum = 35; }
			else if ( rnum == 56 ) { nNewItemID = 2022000; nNewItemNum = 35; }
			else if ( rnum == 57 ) { nNewItemID = 2022003; nNewItemNum = 35; }
			--//晦顫蠱
			else if ( rnum == 58 ) { nNewItemID = 4003000; nNewItemNum = 75; }
			else if ( rnum == 59 ) { nNewItemID = 4010000; nNewItemNum = 18; }
			else if ( rnum == 60 ) { nNewItemID = 4010001; nNewItemNum = 18; }
			else if ( rnum == 61 ) { nNewItemID = 4010002; nNewItemNum = 18; }
			else if ( rnum == 62 ) { nNewItemID = 4010003; nNewItemNum = 18; }
			else if ( rnum == 63 ) { nNewItemID = 4010004; nNewItemNum = 18; }
			else if ( rnum == 64 ) { nNewItemID = 4010005; nNewItemNum = 18; }
			else if ( rnum == 65 ) { nNewItemID = 4010006; nNewItemNum = 12; }
			else if ( rnum == 66 ) { nNewItemID = 4020000; nNewItemNum = 18; }
			else if ( rnum == 67 ) { nNewItemID = 4020001; nNewItemNum = 18; }
			else if ( rnum == 68 ) { nNewItemID = 4020002; nNewItemNum = 18; }
			else if ( rnum == 69 ) { nNewItemID = 4020003; nNewItemNum = 18; }
			else if ( rnum == 70 ) { nNewItemID = 4020004; nNewItemNum = 18; }
			else if ( rnum == 71 ) { nNewItemID = 4020005; nNewItemNum = 18; }
			else if ( rnum == 72 ) { nNewItemID = 4020006; nNewItemNum = 18; }
			else if ( rnum == 73 ) { nNewItemID = 4020007; nNewItemNum = 7; }
			else if ( rnum == 74 ) { nNewItemID = 4020008; nNewItemNum = 7; }
			--//輿僥憮盟
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
			else if ( rnum >= 96 and rnum <= 130 ) { nNewItemID = 2000004; nNewItemNum = 20; }
			else if ( rnum >= 131 and rnum <= 150 ) { nNewItemID = 2000005; nNewItemNum = 10; }
			else if ( rnum >= 151 and rnum <= 180 ) { nNewItemID = 2000002; nNewItemNum = 100; }
			else if ( rnum >= 181 and rnum <= 200 ) { nNewItemID = 2000006; nNewItemNum = 50; }
			else { nNewItemID = 2000003; nNewItemNum = 100; }

			ret = inven->exchange( 0, nNewItemID, nNewItemNum );
			if ( ret != 0 ) self->say( "你確定背包還有足夠的空位嗎？如果背包已滿，我就不能給你獎勵。" );
			else {
				party3_takeawayitem();
				--
				qr->setState( 7020, 1, "1" );
				--//2007 檜漸?挪? 蠡蝶?? 諫猿 籀葬
				cTime = currentTime();
				if ( serverType == 2 ) {
					wsTime = compareTime( cTime, "07/01/09/00/00" );
					weTime =  compareTime( "07/02/04/23/59", cTime );
				} else {
					wsTime = compareTime( cTime, "07/01/16/00/00" );
					weTime =  compareTime( "07/02/04/23/59", cTime );
				}

				if ( wsTime >= 0 and weTime >= 0 ) {
					if ( qr->getState( 9663 ) == 1 ) {
						qr->setComplete( 9663 );
						target->chatMsg(5, "Voc?completou a miss緌 de [Consolar a Deusa]." );
					}
				}
				--//黴濰
				target->transferField( 920011200, "" );
			}
		}
		else self->say( "你確定背包還有足夠的空位嗎？如果背包已滿，我就不能給你獎勵。" );
	} 	
}

--//瑰檣曖 寞(920010500) 港 撲薑
function party3_nQuizAns() {
	quest = FieldSet.get("Party3");

	ans2_1 = random( 0, 5 );
	ans2_2 = random( 0, 5 - ans2_1 );
	ans2_3 = 5 - ans2_1 - ans2_2;

	rand_anw = random( 1, 6 );
	if ( rand_anw == 1 ) {
		quest->setVar( "ans1", ( ans2_1 ));
		quest->setVar( "ans2", ( ans2_2 ));
		quest->setVar( "ans3", ( ans2_3 ));
	} else if ( rand_anw == 2 ) {
		quest->setVar( "ans1", ( ans2_1 ));
		quest->setVar( "ans3", ( ans2_2 ));
		quest->setVar( "ans2", ( ans2_3 ));
	} else if ( rand_anw == 3 ) {
		quest->setVar( "ans2", ( ans2_1 ));
		quest->setVar( "ans1", ( ans2_2 ));
		quest->setVar( "ans3", ( ans2_3 ));
	} else if ( rand_anw == 4 ) {
		quest->setVar( "ans2", ( ans2_1 ));
		quest->setVar( "ans3", ( ans2_2 ));
		quest->setVar( "ans1", ( ans2_3 ));
	} else if ( rand_anw == 5 ) {
		quest->setVar( "ans3", ( ans2_1 ));
		quest->setVar( "ans1", ( ans2_2 ));
		quest->setVar( "ans2", ( ans2_3 ));
	} else {
		quest->setVar( "ans3", ( ans2_1 ));
		quest->setVar( "ans2", ( ans2_2 ));
		quest->setVar( "ans1", ( ans2_3 ));
	}
	return;
}

--//衛謙 檜觼
function s_party3_play() {
	field = self->getField();
	quest = FieldSet.get("Party3");

	if ( field->getID() == 920010000 ) {
		if ( quest->getVar( "prestage_clear" ) != "1" ) {
			if ( target->isPartyBoss() == 1 ) {
				field->effectScreen( "quest/party/clear" );
				field->effectSound( "Grupo1/Completado" );
				self->say( "Obrigado por erguer o meu corpo. Voc瘰 est緌 todos aqui para resgatar a Deusa Minerva, n緌 ? Vou ajudar o m嫞imo que puder. Certo, vou levar voc瘰 at?a entrada da torre." );
				quest->setVar( "prestage_clear", "1" );
				quest->incExpAll( 6000, 7020 );
				quest->transferAll( 920010000, "st00" );
			} else {
				self->say( "Obrigado por erguer o meu corpo. Voc瘰 est緌 todos aqui para resgatar a Deusa Minerva, n緌 ? Vou ajudar o m嫞imo que puder. Agora continue, por favor, com o l獮er do seu grupo mostrando o caminho." );
				return;
			}
		} else {
			self->say( "Certo, vou levar voc瘰 at?a entrada da torre." );
			target->transferField( 920010000, "st00" );			
		}
	--//醞懈瑛
	} else if ( field->getID() == 920010100 ) {
		stage0 = quest->getVar( "stage0_clear" );
		mine = quest->getReactorState( 1, "minerva" );
		--self->say(mine);
		if ( target->isPartyBoss() == 1 ) {
			if ( mine == 6 ) {
				self->say( "非常好！雕像已經復原了，現在只需要帶回#b生命草#k。生命草在女神之塔的庭園裡。現在我就帶你們到那邊吧！" );
				quest->setVar( "stage0_clear", "1" );
				quest->transferAll( 920010800, "in00" );	
				return;
			} else if ( mine == 7 ) {
				self->say( "我無法用言語表達對你們拯救女神的感謝！" );
				return;
			}
		}

		if ( target->isPartyBoss() != 1 ) {
			if ( stage0 == ""  or stage0 == "s" ) {
				party3_out()();
				return;
			} 
		} else {
			if ( stage0 == "" ) {
				party3_help();
				quest->setVar( "stage0_clear", "s" );
				return;
			} else if ( stage0 == "s" ) {
				party3_out();
				return;
			} else if ( stage0 == "1" ) {
				self->say( "得到生命草了嗎？如果完成了，請回去並將它放在女神雕像的正中央。" );
				return;
			}
		}
	--//寞1-骯疇煎
	} else if ( field->getID() == 920010200 ) {
		stage1 = quest->getVar( "stage1_clear" );

		if ( stage1 == "" ) {
			if ( target->isPartyBoss() != 1 ) {
				party3_help();
				return;
			} else {
				party3_help();
				quest->setVar( "stage1_clear", "s" );
				return;
			}
		} else if ( stage1 == "s" ) {
			if ( target->isPartyBoss() != 1 ) {
				party3_help();
				return;
			} else {
				inven = inventory;
				nItem =  inven->itemCount( 4001050 );
				if ( nItem >= 30 ) {
					self->say( "Ohhh! Voc?conseguiu juntar 30 #b#t4001050#s#k! Com isso posso fazer #b#t4001044##k. Pronto, vou fazer agora mesmo!" );
					ret = inven->exchange( 0, 4001050, -nItem, 4001044, 1 );

					if ( ret != 1 ) self->say ( "Por favor, verifique se seu invent嫫io tem espa蔞 livre." );
					else {
						field->effectScreen( "quest/party/clear" );
						field->effectSound( "Grupo1/Completado" );
						quest->setVar( "stage1_clear", "1" );
						quest->incExpAll( 7500, 7020 );
						return;
					}
				} else {
					self->say( "Voc?coletou todos os #b#t4001050#s#k?" );
					return;
				}
			}
		} else {
			self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
			return;
		}
	--//寞2-璽堅
	} else if ( field->getID() == 920010300 ) {
		stage2 = quest->getVar( "stage2_clear" );

		if ( stage2 == "" ) {
			if ( target->isPartyBoss() != 1 ) {
				party3_help();
				return;
			} else {
				party3_help();
				quest->setVar( "stage2_clear", "s" );
				return;
			}
		} else if ( stage2 == "s") {
			 if ( target->isPartyBoss() == 1 ) {
				inven = inventory;
--//			nItem =  inven->itemCount( 4001051 );
				nItem =  inven->itemCount( 4001045 );
				if ( nItem >= 1 ) {
--//					self->say( "Uau! Voc?encontrou #b#t4001045##k" );
--//				ret = inven->exchange( 0, 4001051, -nItem, 4001045, 1 );
--//				if ( ret != 1 ) self->say ( "檣漸饜葬縑 綴蘊檜 氈朝雖 ?挫恉? 爾撮蹂." );
--//				else {
					field->effectScreen( "quest/party/clear" );
					field->effectSound( "Grupo1/Completado" );
					quest->setVar( "stage2_clear", "1" );
					quest->incExpAll( 7500, 7020 );
					return;
				} else {
					self->say( "Voc?coletou #b#t4001045##k?" );
					return;
				}
			}
		} else {
			self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
			return;			
		}
	--//寞3-?獉埣?
	} else if ( field->getID() == 920010400 ) {
		if ( target->isPartyBoss() != 1 ) {
			party3_help();
			return;
		} else {
			if ( quest->getReactorState( 4, "music" ) == 0 ) {
				party3_help();
			} else {
				if ( quest->getVar( "stage3_clear" ) != "1" ) {
					day = dayOfWeek();
					if ( quest->getReactorState( 4, "music" ) == ( day )) {
						self->say( "Sim! Esta ?a m?sica! A Deusa adorava ouvir esta m?sica de vez em quando." );
						quest->setReactorState( 4, "stone3", 1, 0 );
						field->effectScreen( "quest/party/clear" );
						field->effectSound( "Grupo1/Completado" );
						quest->setVar( "stage3_clear", "1" );
						quest->incExpAll( 7500, 7020 );
					} else {
						self->say( "N緌 ?isto." );
					}
				} else {
					self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
					return;
				}
			}
		}
	--//寞4-璋濠?衖? 瑰檣脹寞
	} else if ( field->getID() == 920010500 ) {
		--//璋濠蜃蹺晦
		stage4 = quest->getVar( "stage4_clear" );

		if ( target->isPartyBoss() != 1 ) {
			if ( stage4 != "1" ) {			
				party3_help();
				return;
			} else {
				self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, encontre o peda蔞 da est嫢ua da Deusa em outra sala." );
				return;
			}
		}

		if ( stage4 == "" ) {
			party3_help();
			party3_nQuizAns();
			quest->setVar( "try", "0" );
			quest->setVar( "stage4_clear", "s" );
			--//薑港
--//			say = quest->getVar( "ans1" ) + quest->getVar( "ans2" ) + quest->getVar( "ans3" );
--//			target->chatMsg(5, say );
			return;
		} else if ( stage4 == "s" ) {
			--//譆蟾 撲薑
			if ( quest->getVar( "ans1" ) == "" ) {
				party3_nQuizAns();
			}

			area1 = field->countUserInArea ( "1" );
			area2 = field->countUserInArea ( "2" );
			area3 = field->countUserInArea ( "3" );

			if ( false and( area1 + area2 + area3 != 5) ) {
				self->say( "Voc?vai precisar de 5 pessoas no alto das plataformas." );
				return;
			} 

			co = 0;
			if ( area1 == tonumber( quest->getVar( "ans1" ))) co = co + 1;
			if ( area2 == tonumber( quest->getVar( "ans2" ))) co = co + 1;
			if ( area3 == tonumber( quest->getVar( "ans3" ))) co = co + 1;

			if ( co <0 ) {
				nNum = tonumber( quest->getVar( "try" ) ) +1;
				tried = tonumber( quest->getVar( "try" ) );
				tried = tried + 1;
				quest->setVar( "try", ( tried ) );
				if ( nNum == 6 ) {
					if ( co == 0 ) self->say ( "Esta ?a sua tentativa n?mero" +( nNum ).. ".\r\nTodas estas plataformas t瘱 pesos diferentes.\r\nVoc?s?tem mais uma tentativa, tenha cuidado!" );
					else self->say ( "Esta ?a sua tentativa n?mero" ..  ( nNum ) .. ".\r\nO peso sobre" .. ( co ).. "as plataformas est?correto.\r\nVoc?tem uma tentativa restante, tenha cuidado." );
				} else if ( nNum >= 7 ) {
					--//self->say( "......... ............ ............ ................" );
					
					quest->broadcastMsg( 6, "Voc?falhou e ser?removido da sala." );
					quest->setVar( "try", "0" );
					quest->setVar( "stage4_clear", "" );
					field->transferAll( 920010100, "in03" );
					return;
				} else {
					if ( co == 0 ) self->say ( "Esta ?a sua tentativa n?mero" ..  ( nNum ) .. ".\r\nTodas as plataformas t瘱 pesos diferentes." );
					else self->say ( "Esta ?a sua tentativa n?mero" ..  ( nNum ) .. ".\r\nO peso sobre" .. ( co ).. "as plataformas est?correto." );
				}
				return;
			} else {
				self->say( "Resposta correta. Algo apareceu em cima do altar." );
				field->effectScreen( "quest/party/clear" );
				field->effectSound( "Grupo1/Completado" );
				quest->setReactorState( 5, "stone4", 1, 0 );
				quest->setVar( "stage4_clear", "1" );
				quest->incExpAll( 7500, 7020 );
				return;
			}
		} else if ( stage4 == "1" ) {
			self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, encontre o peda蔞 da est嫢ua da Deusa em outra sala." );
			return;
		}
	--//寞5
	} else if ( field->getID() == 920010600 ) {
		stage5 = quest->getVar( "stage5_clear" );

		if ( target->isPartyBoss() != 1 ) {
			if ( stage5 != "1" ) {			
				party3_help();
				return;
			} else {
				self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
				return;
			}
		}

		if ( stage5 == "" ) {
			party3_help();
			quest->setVar( "stage5_clear", "s" );
			return;
		} else if ( stage5 == "s" ) {
			inven = inventory;
			nItem =  inven->itemCount( 4001052 );
			if ( nItem >= 40 ) {
				self->say( "Ohhh! Voc?conseguiu juntar 40 #b#t4001052#s#k! Com isto posso fazer #b#t4001048##k. Pronto, vou fazer agora mesmo!" );
				ret = inven->exchange( 0, 4001052, -nItem, 4001048, 1 );
				if ( ret != 1 ) self->say ( "Por favor, verifique se seu invent嫫io tem espa蔞 livre." );
				else {
					field->effectScreen( "quest/party/clear" );
					field->effectSound( "Grupo1/Completado" );
					quest->setVar( "stage5_clear", "1" );
					quest->incExpAll( 7500, 7020 );
					return;
				}
			} else {
				self->say( "Voc?coletou o #b#t4001052#s#k?" );
			}
		} else if ( stage5 == "1" ) {
			self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
			return;
		}
	--//螢塭陛朝望
	} else if ( field->getID() == 920010700 ) {
		stage6 = quest->getVar( "stage6_clear" );

		if ( target->isPartyBoss() != 1 ) {
			if ( stage6 != "1" ) {			
				party3_help();
				return;
			} else {
				self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
				return;
			}
		}

		if ( stage6 == "" ) {
			party3_help();
			quest->setVar( "stage6_clear", "s" );
			return;
		} else if ( stage6 == "s" ) {
			ans1 =  quest->getVar( "stage6_ans1" );
			ans2 =  quest->getVar( "stage6_ans2" );
			wans1 = quest->getVar( "stage6_wans1" );
			wans2 = quest->getVar( "stage6_wans2" );
			wans3 = quest->getVar( "stage6_wans3" );

			--target->chatMsg(5, quest->getVar( "room6_ans" )  .. " : Ans1 = " .. ans1 .. ", Ans2= " .. ans2 .. ", wan1 = " .. wans1 .. ", wans2= " ..wans2 ..", wans3 = " ..wans3);
			--target->chatMsg(5, quest->getVar( "room6_ans" )  .. " : Ans1 = " .. quest->getReactorState(11, ans1) .. ", Ans2= " .. quest->getReactorState(11, ans2) .. ", wan1 = " .. quest->getReactorState(11, wans1) .. ", wans2= " ..quest->getReactorState(11, wans2) ..", wans3 = " ..quest->getReactorState(11, wans3));
			if ( quest->getReactorState( 11, ans1 ) == 1 and 
				 quest->getReactorState( 11, ans2) == 1 and 
				 quest->getReactorState( 11, wans1 ) == 0 and 
				 quest->getReactorState( 11, wans2 ) == 0 and 
				 quest->getReactorState( 11, wans3 ) == 0 ) {
				self->say( "Voc?moveu a alavanca com sucesso. Algo apareceu em cima do altar." );
				field->effectScreen( "quest/party/clear" );
				field->effectSound( "Grupo1/Completado" );
				quest->setReactorState( 11, "stone6", 1, 0 );
				quest->setVar( "stage6_clear", "1" );
				quest->incExpAll( 7500, 7020 );
				return;
			} else {
				self->say( "Nada aconteceu, o que significa que voc?n緌 deve ter movido a alavanca correta." );
				return;
			}
		} else if ( stage6 == "1" ) {
			self->say( "Eu acho que n緌 tem mais nada o que fazer nesta sala. Por favor, entre em outra sala." );
			return;
		}
	} else if ( field->getID() == 920010800 ) {
		stageboss = quest->getVar( "stageboss_clear" );

		if ( target->isPartyBoss() != 1 ) {
			party3_out();
		} else {
			inven = inventory;
			nItem = inven->itemCount( 4001055 );--//儅貲曖??
			if ( nItem > 0 ) {
				self->say( "哦，你找到了生命草了！現在我們可以拯救女神了！請回到中央塔，拯救女神！" );
				return;
			} else {
				party3_out();
			}
		}
	} else if ( field->getID() == 920010900 ) {
		party3_help();
	} else if ( field->getID() == 920011000 ) {
		party3_help();
	} else if ( field->getID() == 920011100 ) {
		party3_out();
	} else if ( field->getID() == 920011200 ) {
		party3_out();
	} else if ( field->getID() == 920011200 ) {
		party3_out();
	}
}

--//陝 寞 殮濰
function s_party3_room1() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "stage1_clear" ) == "1" ) {
		say = "Eu acho que n緌 tem mais nada o que fazer nesta sala.";
		target->chatMsg(5, say );
		return;
	}

	if ( target->isPartyBoss() != 1 ) {
		if ( Field.get( 920010200 )->getUserCount() > 0 ) {
			target->playPortalSE();
			target->transferField( 920010200, "st00" );
			return;
		} else {
			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou na <Alameda>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920010200, "st00" );
	}
}

function s_party3_room2() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "stage2_clear" ) == "1" ) {
		say = "N緌 h?mais nada para fazer nesta sala.";
		target->chatMsg(5, say );
		return;
	}

	if ( target->isPartyBoss() != 1 ) {
		if ( Field.get( 920010300 )->getUserCount() > 0 ) {
			target->playPortalSE();
			target->transferField( 920010300, "st00" );
			return;
		} else {
			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou no <Dep鏀ito>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920010300, "st00" );
	}
}

function s_party3_room3() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "stage3_clear" ) == "1" ) {
		say = "N緌 h?mais nada para fazer nesta sala.";
		target->chatMsg(5, say );
		return;
	}

	if ( target->isPartyBoss() != 1 ) {
		if ( Field.get( 920010400 )->getUserCount() > 0 ) {
			target->playPortalSE();
			target->transferField( 920010400, "st00" );
			return;
		} else {
			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou no <Sagu緌>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920010400, "st00" );
	}
}

--//?鷕? 蝶觼董??
function s_party3_room4() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "stage4_clear" ) == "1" ) {
		say = "N緌 h?mais nada para fazer nesta sala.";
		target->chatMsg(5, say );
		return;
	}

	if ( target->isPartyBoss() != 1 ) {
		if ( Field.get( 920010500 )->getUserCount() > 0 ) {
			target->playPortalSE();
			target->transferField( 920010500, "st00" );
			return;
		} else {
			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou na <Sala Lacrada>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920010500, "st00" );
	}
}

function s_party3_room5() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "stage5_clear" ) == "1" ) {
		say = "N緌 h?mais nada para fazer nesta sala.";
		target->chatMsg(5, say );
		return;
	}

	if ( target->isPartyBoss() != 1 ) {
		nNum = Field.get( 920010600 )->getUserCount() + Field.get( 920010601 )->getUserCount() + Field.get( 920010602 )->getUserCount() + Field.get( 920010603 )->getUserCount() + Field.get( 920010604 )->getUserCount();
		if ( nNum > 0 ) {
			target->playPortalSE();
			target->transferField( 920010600, "st00" );
			return;
		} else {
			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou no <Sal緌>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920010600, "st00" );
	}
}

function s_party3_room6() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "stage6_clear" ) == "1" ) {
		say = "N緌 h?mais nada para fazer nesta sala.";
		target->chatMsg(5, say );
		return;
	}

	if ( quest->getVar( "room6_ans" ) != "1" ) {
		sh = make_area_answer(5, 2);
		t = 0;
		k =0;
			
		for i = 0 , 4  {
			st = substring( sh, i, 1 );
			if ( st == "1" ) {
				if ( t == 0 ) {
					quest->setVar( "stage6_ans1", ( i + 1) );
					t = t + 1;
				} else if ( t == 1 ) {
					quest->setVar( "stage6_ans2", ( i  + 1) );
					t = t + 1;
				}
			} else if ( st == "0" ) {
				if ( k == 0 ) {
					quest->setVar( "stage6_wans1", ( i + 1) );
					k = t + 1;
				} else if ( k == 1 ) {
					quest->setVar( "stage6_wans2", ( i + 1) );
					k = t + 1;
				} else if ( k == 2 ) {
					quest->setVar( "stage6_wans3", ( i + 1) );
					k = t + 1;
				}
			}
		}
		quest->setVar( "room6_ans", "1" );
	}



	if ( target->isPartyBoss() != 1 ) {
		if ( Field.get( 920010700 )->getUserCount() > 0 ) {
			target->playPortalSE();
			target->transferField( 920010700, "st00" );
			return;
		} else {
			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou na <Sa獮a para cima>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920010700, "st00" );
	}
}

function s_party3_room8() {
	quest = FieldSet.get("Party3");

	if ( target->isPartyBoss() != 1 ) {
		if ( Field.get( 920011000 )->getUserCount() > 0 ) {
			target->transferField( 920011000, "st00" );
			return;
		} else {
--//			say = "Voc?s?pode entrar nos locais onde se encontra o l獮er do seu grupo.";
--//			target->chatMsg(5, say );
			return;
		}
	} else {
		say = "O l獮er do seu grupo entrou na <Sala da Escurid緌>.";
		quest->broadcastMsg( 6, say );
		target->playPortalSE();
		target->transferField( 920011000, "st00" );
	}
}

--//寞5(920010600) 縑憮 楠渾 ?鷕?
function s_party3_r4pt() {
	quest = FieldSet.get("Party3");

	if (quest->getVar( "r4_rp" ) != "1") {
		quest->setVar( "r4way1", ( random( 1, 3 )));
		quest->setVar( "r4way2", ( random( 1, 3 )));

	--//	say = quest->getVar( "r4way1" ) + quest->getVar( "r4way2" );
	--//	target->chatMsg(5, say );
		quest->setVar( "r4_rp", "1" );
	}

	if ( portal.getPortalID == 11 ) {
		if ( quest->getVar( "r4way1" ) == "1") {
			target->transferField( -1, "np00" );		
		} else target->transferField(  -1, "np02" );		
		return;
	} else if ( portal.getPortalID == 12 ) {
		if (quest->getVar( "r4way1" ) == "2") {
			target->transferField(  -1, "np00" );		
		} else target->transferField( -1, "np02" );		
		return;
	} else if ( portal.getPortalID == 13 ) {
		if (quest->getVar( "r4way1" ) == "3") {
			target->transferField(  -1, "np00" );		
		} else target->transferField( -1, "np02" );		
		return;
	} else if ( portal.getPortalID == 14 ) {
		if (quest->getVar( "r4way2" ) == "1") {
			target->transferField(  -1, "np01" );		
		} else target->transferField( -1, "np02" );		
		return;
	} else if ( portal.getPortalID == 15 ) {
		if (quest->getVar( "r4way2" ) == "2") {
			target->transferField(  -1, "np01" );		
		} else target->transferField( -1, "np02" );		
		return;
	} else if ( portal.getPortalID == 16 ) {
		if (quest->getVar( "r4way2" ) == "3") {
			target->transferField(  -1, "np01" );		
		} else target->transferField( -1, "np02" );		
		return;
	}
}

function s_party3_r6pt() {
	quest = FieldSet.get("Party3");

	if ( quest->getVar( "r6_rp" ) != "1") {
		quest->setVar( "r6way1", ( random( 1, 4 )));
		quest->setVar( "r6way2", ( random( 1, 4 )));
		quest->setVar( "r6way3", ( random( 1, 4 )));
		quest->setVar( "r6way4", ( random( 1, 4 )));
		quest->setVar( "r6way5", ( random( 1, 4 )));
		quest->setVar( "r6way6", ( random( 1, 4 )));
		quest->setVar( "r6way7", ( random( 1, 4 )));
		quest->setVar( "r6way8", ( random( 1, 4 )));
		quest->setVar( "r6way9", ( random( 1, 4 )));
		quest->setVar( "r6way10", ( random( 1, 4 )));
		quest->setVar( "r6way11", ( random( 1, 4 )));
		quest->setVar( "r6way12", ( random( 1, 4 )));
		quest->setVar( "r6way13", ( random( 1, 4 )));
		quest->setVar( "r6way14", ( random( 1, 4 )));
		quest->setVar( "r6way15", ( random( 1, 4 )));
		quest->setVar( "r6way16", ( random( 1, 4 )));

		--//薑港寰頂
	--//	say = quest->getVar( "r6way1" ) + quest->getVar( "r6way2" ) + quest->getVar( "r6way3" ) + quest->getVar( "r6way4" ).. "-" .. quest->getVar( "r6way5" ) + quest->getVar( "r6way6" ) + quest->getVar( "r6way7" ) + quest->getVar( "r6way8" ).. "-" .. quest->getVar( "r6way9" ) + quest->getVar( "r6way10" ) + quest->getVar( "r6way11" ) + quest->getVar( "r6way12" ).. "-" .. quest->getVar( "r6way13" ) + quest->getVar( "r6way14" ) + quest->getVar( "r6way15" ) + quest->getVar( "r6way16" );
	--//	target->chatMsg(5, say );

		quest->setVar( "r6_rp", "1" );
	}

	target->playPortalSE();
	--//類1
	if ( portal.getPortalID == 24 ) {
		if ( quest->getVar( "r6way1" ) == "1" ) target->transferField( -1, "np00" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 25 ) {
		if ( quest->getVar( "r6way1" ) == "2") target->transferField( -1, "np00" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 26 ) {
		if (quest->getVar( "r6way1" ) == "3") target->transferField( -1, "np00" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 27 ) {
		if (quest->getVar( "r6way1" ) == "4") target->transferField( -1, "np00" );		
		else target->transferField( -1, "np16" );		
		return;
	--//類2
	} else if ( portal.getPortalID == 28 ) {
		if (quest->getVar( "r6way2" ) == "1") target->transferField( -1, "np01" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 29 ) {
		if (quest->getVar( "r6way2" ) == "2") target->transferField( -1, "np01" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 30 ) {
		if (quest->getVar( "r6way2" ) == "3") target->transferField( -1, "np01" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 31 ) {
		if (quest->getVar( "r6way2" ) == "4") target->transferField( -1, "np01" );		
		else target->transferField( -1, "np16" );		
		return;
	--//類3
	} else if ( portal.getPortalID == 32 ) {
		if (quest->getVar( "r6way3" ) == "1") target->transferField( -1, "np02" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 33 ) {
		if (quest->getVar( "r6way3" ) == "2") target->transferField( -1, "np02" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 34 ) {
		if (quest->getVar( "r6way3" ) == "3") target->transferField( -1, "np02" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 35 ) {
		if (quest->getVar( "r6way3" ) == "4") target->transferField( -1, "np02" );		
		else target->transferField( -1, "np16" );		
		return;
	--//類4
	} else if ( portal.getPortalID == 36 ) {
		if (quest->getVar( "r6way4" ) == "1") target->transferField( -1, "np03" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 37 ) {
		if (quest->getVar( "r6way4" ) == "2") target->transferField( -1, "np03" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 38 ) {
		if (quest->getVar( "r6way4" ) == "3") target->transferField( -1, "np03" );		
		else target->transferField( -1, "np16" );		
		return;
	} else if ( portal.getPortalID == 39 ) {
		if (quest->getVar( "r6way4" ) == "4") target->transferField( -1, "np03" );		
		else target->transferField( -1, "np16" );		
		return;
	--//類5
	} else if ( portal.getPortalID == 40 ) {
		if (quest->getVar( "r6way5" ) == "1") target->transferField( -1, "np04" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 41 ) {
		if (quest->getVar( "r6way5" ) == "2") target->transferField( -1, "np04" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 42 ) {
		if (quest->getVar( "r6way5" ) == "3") target->transferField( -1, "np04" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 43 ) {
		if (quest->getVar( "r6way5" ) == "4") target->transferField( -1, "np04" );		
		else target->transferField( -1, "np03" );		
		return;
	--//類6
	} else if ( portal.getPortalID == 44 ) {
		if (quest->getVar( "r6way6" ) == "1") target->transferField( -1, "np05" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 45 ) {
		if (quest->getVar( "r6way6" ) == "2") target->transferField( -1, "np05" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 46 ) {
		if (quest->getVar( "r6way6" ) == "3") target->transferField( -1, "np05" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 47 ) {
		if (quest->getVar( "r6way6" ) == "4") target->transferField( -1, "np05" );		
		else target->transferField( -1, "np03" );		
		return;

	} else if ( portal.getPortalID == 48 ) {
		if (quest->getVar( "r6way7" ) == "1") target->transferField( -1, "np06" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 49 ) {
		if (quest->getVar( "r6way7" ) == "2") target->transferField( -1, "np06" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 50 ) {
		if (quest->getVar( "r6way7" ) == "3") target->transferField( -1, "np06" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 51 ) {
		if (quest->getVar( "r6way7" ) == "4") target->transferField( -1, "np06" );		
		else target->transferField( -1, "np03" );		
		return;

	} else if ( portal.getPortalID == 52 ) {
		if (quest->getVar( "r6way8" ) == "1") target->transferField( -1, "np07" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 53 ) {
		if (quest->getVar( "r6way8" ) == "2") target->transferField( -1, "np07" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 54 ) {
		if (quest->getVar( "r6way8" ) == "3") target->transferField( -1, "np07" );		
		else target->transferField( -1, "np03" );		
		return;
	} else if ( portal.getPortalID == 55 ) {
		if (quest->getVar( "r6way8" ) == "4") target->transferField( -1, "np07" );		
		else target->transferField( -1, "np03" );		
		return;

	} else if ( portal.getPortalID == 56 ) {
		if (quest->getVar( "r6way9" ) == "1") target->transferField( -1, "np08" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 57 ) {
		if (quest->getVar( "r6way9" ) == "2") target->transferField( -1, "np08" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 58 ) {
		if (quest->getVar( "r6way9" ) == "3") target->transferField( -1, "np08" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 59 ) {
		if (quest->getVar( "r6way9" ) == "4") target->transferField( -1, "np08" );		
		else target->transferField( -1, "np07" );		
		return;

	} else if ( portal.getPortalID == 60 ) {
		if (quest->getVar( "r6way10" ) == "1") target->transferField( -1, "np09" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 61 ) {
		if (quest->getVar( "r6way10" ) == "2") target->transferField( -1, "np09" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 62 ) {
		if (quest->getVar( "r6way10" ) == "3") target->transferField( -1, "np09" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 63 ) {
		if (quest->getVar( "r6way10" ) == "4") target->transferField( -1, "np09" );		
		else target->transferField( -1, "np07" );		
		return;	
		
	} else if ( portal.getPortalID == 64 ) {
		if (quest->getVar( "r6way11" ) == "1") target->transferField( -1, "np10" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 65 ) {
		if (quest->getVar( "r6way11" ) == "2") target->transferField( -1, "np10" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 66 ) {
		if (quest->getVar( "r6way11" ) == "3") target->transferField( -1, "np10" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 67 ) {
		if (quest->getVar( "r6way11" ) == "4") target->transferField( -1, "np10" );		
		else target->transferField( -1, "np07" );		
		return;

	} else if ( portal.getPortalID == 68 ) {
		if (quest->getVar( "r6way12" ) == "1") target->transferField( -1, "np11" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 69 ) {
		if (quest->getVar( "r6way12" ) == "2") target->transferField( -1, "np11" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 70 ) {
		if (quest->getVar( "r6way12" ) == "3") target->transferField( -1, "np11" );		
		else target->transferField( -1, "np07" );		
		return;
	} else if ( portal.getPortalID == 71 ) {
		if (quest->getVar( "r6way12" ) == "4") target->transferField( -1, "np11" );		
		else target->transferField( -1, "np07" );		
		return;

	} else if ( portal.getPortalID == 72 ) {
		if (quest->getVar( "r6way13" ) == "1") target->transferField( -1, "np12" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 73) {
		if (quest->getVar( "r6way13" ) == "2") target->transferField( -1, "np12" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 74 ) {
		if (quest->getVar( "r6way13" ) == "3") target->transferField( -1, "np12" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 75 ) {
		if (quest->getVar( "r6way13" ) == "4") target->transferField( -1, "np12" );		
		else target->transferField( -1, "np11" );		
		return;

	} else if ( portal.getPortalID == 76 ) {
		if (quest->getVar( "r6way14" ) == "1") target->transferField( -1, "np13" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 77 ) {
		if (quest->getVar( "r6way14" ) == "2") target->transferField( -1, "np13" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 78 ) {
		if (quest->getVar( "r6way14" ) == "3") target->transferField( -1, "np13" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 79 ) {
		if (quest->getVar( "r6way14" ) == "4") target->transferField( -1, "np13" );		
		else target->transferField( -1, "np11" );		
		return;

	} else if ( portal.getPortalID == 80 ) {
		if (quest->getVar( "r6way15" ) == "1") target->transferField( -1, "np14" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 81 ) {
		if (quest->getVar( "r6way15" ) == "2") target->transferField( -1, "np14" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 82 ) {
		if (quest->getVar( "r6way15" ) == "3") target->transferField( -1, "np14" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 83 ) {
		if (quest->getVar( "r6way15" ) == "4") target->transferField( -1, "np14" );		
		else target->transferField( -1, "np11" );		
		return;
	--//窕渠晦
	} else if ( portal.getPortalID == 84 ) {
		if (quest->getVar( "r6way16" ) == "1") target->transferField( -1, "np15" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 85 ) {
		if (quest->getVar( "r6way16" ) == "2") target->transferField( -1, "np15" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 86 ) {
		if (quest->getVar( "r6way16" ) == "3") target->transferField( -1, "np15" );		
		else target->transferField( -1, "np11" );		
		return;
	} else if ( portal.getPortalID == 87 ) {
		if (quest->getVar( "r6way16" ) == "4") target->transferField( -1, "np15" );		
		else target->transferField( -1, "np11" );		
		return;
	}
}

--//陝寞縑憮曖 黴濰
function s_party3_roomout() {
	quest = FieldSet.get("Party3");

	if ( target->isPartyBoss() != 1 ) {
		say = "Apenas o l獮er do grupo pode tomar a decis緌 de sair desta sala ou n緌.";
		target->chatMsg(5, say );
		return;
	} else  {
		field = portal.field;
		if ( field->getID() == 920010200 ) {
			retportal = "in00"; 
			retst = "<Walkway>";
		} else if ( field->getID() == 920010300 ) {
			retportal = "in01";
			retst = "<Storage>";
		} else if ( field->getID() == 920010400 ) {
			retportal = "in02";
			retst = "<Lobby>";
		} else if ( field->getID() == 920010500 ) {
			retportal = "in03";
			retst = "<Sala Lacrada>";
		} else if ( field->getID() == 920010600 ) {
			retportal = "in04";
			retst= "<Sal緌>";
			if ( Field.get( 920010601 )->getUserCount() != 0 or Field.get( 920010602 )->getUserCount() != 0 or Field.get( 920010603 )->getUserCount() != 0 or Field.get( 920010604 )->getUserCount() != 0 ) {
				say = "Voc?n緌 pode sair porque algu幦 no seu grupo ainda est?na sala.";
				target->chatMsg(5, say );
				return;
			}
		} else if ( field->getID() == 920010700 ) {
			retportal = "in05";	
			retst = "<Para Cima>";
		} else if ( field->getID() == 920011000 ) {
			retportal = "in06";
			retst = "<Sala da Escurid緌>";
		}

		say = "O l獮er do grupo saiu" .. retst.. ".";
		quest->broadcastMsg( 6, say );
		field->transferAll( 920010100, retportal );
	}
}

function s_party3_gardenin() {
	quest = FieldSet.get("Party3");
	if ( target->isPartyBoss() != 1 ) {
		say = "Apenas o l獮er do grupo pode tomar a decis緌 de sair desta sala ou n緌.";
		target->chatMsg(5, say );
	} else {
		inven = inventory;
		if ( inven->itemCount( 4001055 ) > 0 ) {
			quest->transferAll( 920010100, "st02" );
		} else {
			say = "Precisamos do poder da Erva da Vida.";
			target->chatMsg(5, say );
			return;
		}
	}
}

--//馬螟1~3
function s_party3_jail1() {
	say = ( portal.getPortalID );
--//	target->chatMsg(5, say );	
	
	uNum = Field.get( 920010910 )->getUserCount() + Field.get( 920010911 )->getUserCount() + Field.get( 920010912 )->getUserCount();
	if ( uNum > 0 ) {
		say = "Algu幦 j?est?l?dentro.";
		target->chatMsg(5, say );
		return;
	} else target->transferField( 920010910, "out00" );
}

function s_party3_jail2() {
	say = ( portal.getPortalID );
--//	target->chatMsg(5, say );	

	uNum = Field.get( 920010920 )->getUserCount() + Field.get( 920010921 )->getUserCount() + Field.get( 920010922 )->getUserCount();
	if ( uNum > 0 ) {
		say = "Algu幦 j?est?l?dentro.";
		target->chatMsg(5, say );
		return;
	} else target->transferField( 920010920, "out00" );
}

function s_party3_jail3() {
	say = ( portal.getPortalID );
--//	target->chatMsg(5, say );	
	
	uNum = Field.get( 920010930 )->getUserCount() + Field.get( 920010931 )->getUserCount() + Field.get( 920010932 )->getUserCount();
	if ( uNum > 0 ) {
		say = "Algu幦 j?est?l?dentro.";
		target->chatMsg(5, say );
		return;
	} else target->transferField( 920010930, "out00" );
}

function s_party3_jailin() {
	field = portal.field;
	if ( field->getID() == 920010910 ) {
		if ( field->getMobCount( 9300044 ) == 0 ) {
			 target->transferField( 920010912, "out00" );
		} else target->transferField( 920010911, "out00" );
	} else if ( field->getID() == 920010920 ) {
		if ( field->getMobCount( 9300044 ) == 0 ) {
			 target->transferField( 920010922, "out00" );
		} else target->transferField( 920010921, "out00" );
	} else if ( field->getID() == 920010930 ) {
		if ( field->getMobCount( 9300044 ) == 0 ) {
			 target->transferField( 920010932, "out00" );
		} else target->transferField( 920010931, "out00" );
	} 
}

