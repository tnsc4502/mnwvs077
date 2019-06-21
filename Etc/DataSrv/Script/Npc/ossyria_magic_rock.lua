require "./DataSrv/Script/sysDef"


--//The Magic Rock @ Orbis 
function s_ossyria3_1() {
    inven = inventory; 

    if ( inven->itemCount( 4001019 ) >= 1 ) { 
        ret1 = self->askYesNo( "你可以使用#b#t4001019##k來啟動#b#p2012014##k。是否要直接傳送到#b#p2012015##k附近的地點？" ); 
        if ( ret1 != 0 ) { 
            ret2 = inven->exchange( 0, 4001019, -1 ); 
            if ( ret2 != 0 ) self->say( "無法啟動#b#p2012014##k，因為所需要的#b#t4001019##k不足。" ); 
            else target->transferField( 200082100, "sp" ); 
        } 
    } 
    else self->say( "這個#b#p2012014##k可以把你傳送到#b#p2012015##k所在地附近，但是必須要有對應的卷軸才能啟動傳送。" ); 
} 

--//The Magic Rock @ El Nath 
function s_ossyria3_2() {
    inven = inventory; 

    if ( inven->itemCount( 4001019 ) >= 1 ) { 
        ret1 = self->askYesNo( "你可以使用#b#t4001019##k來啟動#b#p2012015##k。是否要直接傳送到#b#p2012014##k附近的地點？" ); 
        if ( ret1 != 0 ) { 
            ret2 = inven->exchange( 0, 4001019, -1 ); 
            if ( ret2 != 0 ) self->say( "無法啟動#b#p2012015##k，因為所需要的#b#t4001019##k不足。" ); 
            else target->transferField( 200080200, "sp" ); 
        } 
    } 
    else self->say( "這個#b#p2012015##k可以把你傳送到#b#p2012014##k所在地附近，但是必須要有對應的卷軸才能啟動傳送。"); 
} 