require "./DataSrv/Script/sysDef"

math.randomseed(os.time())

quest = FieldSet.get("Party1")
fieldID = self->field()->getID()
field = self->field()

desc_stage2 = "¶Ù¡AÅwªï¨Ó¨ì²Ä¤GÃö¥d¡C±z·|¬Ý¨ì¦b§Ú®ÇÃä¦³¼Æ±øÃ·¤l¡A½Ð§ä¥X3­Ó¥¿½TªºÃ·¤l¨Ã¥Bª¦¦b¤W­±¡A¦AÅý¶¤ªø»P§Ú¥æ½Í¡A¦pªG¥þ¼Æ¥¿½T¡A´N¯à¶i¤J¤U¤@Ãö¥d¡Cª`·N¡Aª¦±o¤Ó§C¥i¯à¤£·|³Q­pºâ¡C"
desc_stage3 = "¶Ù¡AÅwªï¨Ó¨ì²Ä¤TÃö¥d¡C±z·|¬Ý¨ì¦b§Ú®ÇÃä¦³¼Æ­Ó¤ì±í¡A½Ð§ä¥X3­Ó¥¿½Tªº¤ì±í¨Ã¥B¯¸¦b¤W­±¡A¦AÅý¶¤ªø»P§Ú¥æ½Í¡A¦pªG¥þ¼Æ¥¿½T¡A´N¯à¶i¤J¤U¤@Ãö¥d¡Cª`·N¡A¯¸ªº¤Ó¾aªñÃä½t¥i¯à¤£·|³Q­pºâ¡C"
desc_stage4 = "¶Ù¡AÅwªï¨Ó¨ì²Ä¥|Ãö¥d¡C±z·|¬Ý¨ì¦b§Ú®ÇÃä¦³¼Æ­Ó¤ì±í¡A½Ð§ä¥X3­Ó¥¿½Tªº¤ì±í¨Ã¥B¯¸¦b¤W­±¡A¦AÅý¶¤ªø»P§Ú¥æ½Í¡A¦pªG¥þ¼Æ¥¿½T¡A´N¯à¶i¤J¤U¤@Ãö¥d¡Cª`·N¡A¯¸ªº¤Ó¾aªñÃä½t¥i¯à¤£·|³Q­pºâ¡C"
desc_stage5 = "«¢Åo¡AÅwªï¨Ó¨ì³Ì«á¤@Ãö¡C½Ð¦b¦a¹Ï¤¤§ä¥X¶W¯Åºñ¤ôÆF¡CÀ»±Ñ¦a¹Ï¤¤ªº©Ò¦³©Çª«¡A¦¬¶°#b³q¦æÃÒ#k¨Ã¥B¥æµ¹§Ú¡C·í±zÀò±o¤F#b³q¦æÃÒ#k¡A½Ð²Î¤@¥æµ¹¶¤ªø¡A¨Ã¥Ñ¶¤ªø¥æµ¹§Ú¡C½Ð¤p¤ß¡A¥|©Pªº©Çª«¬Ýþ¨ÓÁöµM«Ü¼ô±x¡A¦ý«o§ó¥[±j¤j¡A½Ð°È¥²¤p¤ß¡C¯¬±z¦n¹B¡I\r\n·í±z§¹¦¨¤F©Ò¦³ªº¥ô°È¡A½Ð§ä§Ú»â¨ú¼úÀy¡C"

function makeAnswer(numCount, answerCount) 
    rndSeq = randomSeq(1, numCount)
    ret = ""
    for k, v in pairs(rndSeq) do
        if(v > answerCount) then
            ret = ret .. '0'
        else
            ret = ret .. '1'
        end
    end
    return ret
end

function areaCheck(areaCount, answerCount)
    ret = ""
    areaCount = areaCount - 1
    count = 0
    for i = 0, areaCount do
        inArea = field->countUserInArea(i)
        count = count + inArea
        ret = ret .. inArea
    end
    if(count ~= answerCount) then return 0 end
    return ret
end

function stageCleared()
    field->effectScreen( "quest/party/clear" )
    field->effectSound( "Party1/Clear" )
    field->effectObject( "gate" )
    field->enablePortal( "next00", 1)
end

function party1_personal()
    charKey = userID .. "_"
    prob = quest->getVar(charKey)
    if(prob == "clear") then self->say("½Ðµ¥«Ý¨ä¥L¤H§¹¦¨¡C") return end
    q = prob
    if(q == "") then q = random(1, 7) end
    desc = ""
    ans = ""
    if(tonumber(q) == 1) then desc = "¦b·¬¤§¨¦¥@¬É¸Ì¡A#r¼C¤h­n¤@Âà®É¡A³Ì§Cªºµ¥¯Å¼Æ¦r¬O¦h¤Ö¡H#k¡C½Ð§â¥¿½Tªº²¼¨é§ä¨Ó¡C"; ans = 10
    elseif(tonumber(q) == 2) then desc = "¦b·¬¤§¨¦¥@¬É¸Ì¡A#r¼C¤h­n¤@Âà®É¡A³Ì§Cªº¤O¶q¼Æ¦r¬O¦h¤Ö¡H#k¡C½Ð§â¥¿½Tªº²¼¨é§ä¨Ó¡C"; ans = 35
    elseif(tonumber(q) == 3) then desc = "¦b·¬¤§¨¦¥@¬É¸Ì¡A#rªk®v­n¶i¦æ¤@Âà®É¡A³Ì§Cªº´¼¤O¼Æ¦r¬O¦h¤Ö¡H#k¡C½Ð§â¥¿½Tªº²¼¨é§ä¨Ó¡C"; ans = 25
    elseif(tonumber(q) == 4) then desc = "¦b·¬¤§¨¦¥@¬É¸Ì¡A#r¤}½b¤â­n¤@Âà®É¡A³Ì§Cªº±Ó±¶¼Æ¦r¬O¦h¤Ö¡H#k¡C½Ð§â¥¿½Tªº²¼¨é§ä¨Ó¡C" ans = 25
    elseif(tonumber(q) == 5) then desc = "¦b·¬¤§¨¦¥@¬É¸Ì¡A#r­n¶i¦æ¤GÂà®É¡A³Ì§Cªºµ¥¯Å¼Æ¦r¬O¦h¤Ö¡H#k¡C½Ð§â¥¿½Tªº²¼¨é§ä¨Ó¡C" ans = 30
    elseif(tonumber(q) == 6) then desc = "¦b·¬¤§¨¦¥@¬É¸Ì¡A#rªk®v­n¶i¦æ¤@Âà®É¡A³Ì§Cªºµ¥¯Å¼Æ¦r¬O¦h¤Ö¡H#k¡C½Ð§â¥¿½Tªº²¼¨é§ä¨Ó¡C" ans = 8 
    end

    if(prob == "") then
        quest->setVar( charKey, q )
        self->say("Åwªï¨Ó¨ì²Ä¤@Ãö¥d¡A±µ¤U¨Ó§Ú·|¸ß°Ý¨C­Ó²Õ­û¤@¹D°ÝÃD¡A½Ð¨C­Ó¤H¥´­ËÆs³½©Ç¨Ã¦¬¶°²¼¨é¡A¼Æ¶q­n©M§Ú©Ò´£¥Xªº°ÝÃDªºµª®×¤@¼Ë¦h¡AµM«á§â#b²¼¨é#k¦¬¶°¹L¨Ó¡C·Ç³Æ¦n±µ¨ü°ÝÃD¤F¶Ü¡H")
        self->say(desc)
        return
    end

    if(inventory->itemCount(4001007) == ans) then
        if(inventory->exchange(0, 4001007, -ans, 4001008, 1) ~= 0) then
            self->say("½Ð½T»{±zªº­I¥]ÁÙ¦³¨¬°÷ªºªÅ¶¡¡C")
            return
        end
        quest->setVar(charKey, "clear")
        self->say("¼Æ¶q¥¿½T¡I¨º§Ú²{¦b´N§â#b³q¦æÃÒ#kµ¹±z¡I½Ð¥æµ¹¶¤¥î¤¤ªº¶¤ªø¡C")
    else
        self->say("«Ü©êºp¡A³o¤£¬O¥¿½Tªºµª®×¡I¼Æ¶q­n©M§Ú©Ò´£¥Xªº°ÝÃDªºµª®×¤@¼Ë¦h¡C½Ð®Ú¾Ú°ÝÃD¨ú±o¥¿½T¼Æ¶qªº²¼¨é¡C")
        self->say(desc) 
    end
end

function party1_stage1()
    stage = quest->getVar("stage")
    ansCount = quest->getUserCount() - 1

    if(stage == "") then
        self->say("¶Ù¡CÅwªï¨Ó¨ì²Ä¤@Ãö¥d¡A¬Ý¬Ý³o¸Ìªº¥|©P¡A¥Rº¡µÛÆs³½©Ç¡A±z­Ì¥²¶·¦P¤ß¨ó¤O¥´±Ñ³o¨ÇÆs³½©Ç¡A³o¨ÇÆs³½©Ç·|±¼¸¨#b²¼¨é#k¡C°£¤F¶¤ªø¤§¥~¡A¨C­Ó²Õ­ûmÀ³¸Ó»P§Ú½Í¸Ü¡A§Ú·|µ¹¥L­Ì¤@­ÓÂ²³æªº°ÝÃD¡A¥L­Ì¥²¶·¦¬¶°¸ò°ÝÃDµª®×¤@¼Ë¼Æ¶qªº#b²¼¨é#k¡A±µµÛ§Ú´N·|µ¹¥L­Ì#b³q¦æÃÒ#k¡A·í©Ò¦³ªº²Õ­ûm¦¬¶°¨ì¤F#b³q¦æÃÒ#k¡A¥þ³¡¥æµ¹¶¤ªø¡A¨Ã¥B¥Ñ¶¤ªø¦A¦¸»P§Ú½Í¸Ü¡A´N¥i¥H§¹¦¨³o­ÓÃö¥d¡A®É¶¡¦³­­¡A½Ð§â´¤®É¶¡¡A¯¬±z­Ì¦n¹B¡C")
        quest->setVar("stage", "1")
        return
    end

    if(stage ~= "1") then self->say("¶Ç°eªù¤w¸g¶}±Ò¡A½Ð¥[§Ö¸}¨B«e©¹¤U¤@­ÓÃö¥d¡I") return end
    if(inventory->itemCount(4001008) < ansCount) then 
        self->say("«Ü©êºp¡A±z©Ò¦¬¶°ªº³q¦æÃÒ¼Æ¶q¤£¨¬¡C¦¬¶°ªº³q¦æÃÒ¼Æ¶q¬O¦©°£¶¤ªø«á²Õ­ûªº¼Æ¶q¡A¤]´N¬O " .. ansCount .. " ±i¤~¥i¥H§¹¦¨Ãö¥d¡C½Ð±zªº²Õ­ûºÉ§Ö±o¥X°ÝÃDªºµª®×¡A¦¬¶°»Pµª®×¼Æ¶q¤@¼Ë¦hªº#b²¼¨é#k¡AµM«á±N#b³q¦æÃÒ#k¥æµ¹±z¡C")
    else
        if(inventory->exchange(0, 4001008, -ansCount) ~= 0) then return end
        stageCleared()
        quest->incExpAll(100)
        quest->setVar( "stage", "2" )
    end
end

function party1_stage2()
    if(quest->getVar("stage") ~= "2") then return end
    question = quest->getVar("stage2_q")
    if(question == "") then
        self->say(desc_stage2)
        question = makeAnswer(4, quest->getUserCount())
        quest->setVar("stage2_q", question)
        return
    end
    answer = areaCheck(4, 3)
    if(answer == 0) then
        self->say("½Ð½T«O¨¬°÷ªºª±®a¤w¸g¦bÃ·¤l¤W¡C" .. makeAnswer(6, 3))
    elseif(question ~= answer) then
        field->effectScreen("quest/party/wrong_kor")
        field->effectSound("Party1/Failed")
    else
        stageCleared()
        quest->incExpAll(200)
        quest->setVar( "stage", "3" )
    end
end

function party1_stage3()
    if(quest->getVar("stage") ~= "3") then return end
    question = quest->getVar("stage3_q")
    if(question == "") then
        self->say(desc_stage3)
        question = makeAnswer(5, quest->getUserCount())
        quest->setVar("stage3_q", question)
        return
    end
    answer = areaCheck(5, 3)
    if(answer == 0) then
        self->say("½Ð½T«O¨¬°÷ªºª±®a¤w¸g¦b¤ì±í¤W¡C")
    elseif(question ~= answer) then
        field->effectScreen("quest/party/wrong_kor")
        field->effectSound("Party1/Failed")
    else
        stageCleared()
        quest->incExpAll(400)
        quest->setVar( "stage", "4" )
    end
end

function party1_stage4()
    if(quest->getVar("stage") ~= "4") then return end
    question = quest->getVar("stage4_q")
    if(question == "") then
        self->say(desc_stage4)
        question = makeAnswer(6, quest->getUserCount())
        quest->setVar("stage4_q", question)
        return
    end
    answer = areaCheck(6, 2)
    if(answer == 0) then
        self->say("½Ð½T«O¨¬°÷ªºª±®a¤w¸g¦bÃ·¤l¤W¡C")
    elseif(question ~= answer) then
        field->effectScreen("quest/party/wrong_kor")
        field->effectSound("Party1/Failed")
    else
        stageCleared()
        quest->incExpAll(800)
        quest->setVar( "stage", "5" )
    end
end

function party1_stage5()
    if(quest->getVar("stage") ~= "5") then return end
    count = inventory->itemCount(4001008, 10)
    if(count ~= 10) then
        self->say(desc_stage5)
    else
        inventory->exchange(0, 4001008, -count)
        stageCleared()
        quest->incExpAll(1500)
        quest->setVar( "stage", "5" )
        self->say("³o¬O¥i¥H³q©¹³Ì«áªº¼úÀyÃö¥dªº¶Ç°eªù¡C¸Ì­±ªº©Çª«±z¤@©w¯à»´ÃP¥´±Ñ¡A½Ð¦b®É¶¡­­¨î¤ººÉ¶qªºÀ»±Ñ©Çª«¡A±z¤]¥i¥H¤¤³~´NÂ÷¶}¡C³Ì«á¡AÁÙ¬O®¥³ß±z¡I¦A¨£¤F...")
    end
end

function party1_help()
    if ( fieldID == 103000801 ) then self->say(desc_stage2)
    elseif ( fieldID == 103000802 ) then self->say(desc_stage3)
    elseif ( fieldID == 103000803 ) then self->say(desc_stage4) 
    elseif ( fieldID == 103000804 ) then self->say(desc_stage5) 
    end
end

function party1_reward()
    rnum = random( 0, 39 ) 
    nNewItemID = 0 
    nNewItemNum = 0 
    if ( rnum == 0 )  then
        nNewItemID = 2000004 
        nNewItemNum = 5 
    elseif ( rnum == 1 )  then
        nNewItemID = 2000001 
        nNewItemNum = 100 
    elseif ( rnum == 2 )  then
        nNewItemID = 2000002 
        nNewItemNum = 70 
    elseif ( rnum == 3 )  then
        nNewItemID = 2000003 
        nNewItemNum = 100 
    elseif ( rnum == 4 )  then
        nNewItemID = 2000006 
        nNewItemNum = 50 
    elseif ( rnum == 5 )  then
        nNewItemID = 2022000 
        nNewItemNum = 15 
    elseif ( rnum == 6 )  then
        nNewItemID = 2022003 
        nNewItemNum = 15 
    elseif ( rnum == 7 )  then
        nNewItemID = 2040002 
        nNewItemNum = 1 
    elseif ( rnum == 8 )  then
        nNewItemID = 2040402 
        nNewItemNum = 1 
    elseif ( rnum == 9 )  then
        nNewItemID = 2040502 
        nNewItemNum = 1 
    elseif ( rnum == 10 )  then
        nNewItemID = 2040505 
        nNewItemNum = 1 
    elseif ( rnum == 11 )  then
        nNewItemID = 2040602 
        nNewItemNum = 1 
    elseif ( rnum == 12 )  then
        nNewItemID = 2040802 
        nNewItemNum = 1 
    elseif ( rnum == 13 )  then
        nNewItemID = 4003000 
        nNewItemNum = 30 
    elseif ( rnum == 14 )  then
        nNewItemID = 4010000 
        nNewItemNum = 8 
    elseif ( rnum == 15 )  then
        nNewItemID = 4010001 
        nNewItemNum = 8 
    elseif ( rnum == 16 )  then
        nNewItemID = 4010002 
        nNewItemNum = 8 
    elseif ( rnum == 17 )  then
        nNewItemID = 4010003 
        nNewItemNum = 8 
    elseif ( rnum == 18 )  then
        nNewItemID = 4010004 
        nNewItemNum = 8 
    elseif ( rnum == 19 )  then
        nNewItemID = 4010005 
        nNewItemNum = 8 
    elseif ( rnum == 20 )  then
        nNewItemID = 4010006 
        nNewItemNum = 5 
    elseif ( rnum == 21 )  then
        nNewItemID = 4020000 
        nNewItemNum = 8 
    elseif ( rnum == 22 )  then
        nNewItemID = 4020001 
        nNewItemNum = 8 
    elseif ( rnum == 23 )  then
        nNewItemID = 4020002 
        nNewItemNum = 8 
    elseif ( rnum == 24 )  then
        nNewItemID = 4020003 
        nNewItemNum = 8 
    elseif ( rnum == 25 )  then
        nNewItemID = 4020004 
        nNewItemNum = 8 
    elseif ( rnum == 26 )  then
        nNewItemID = 4020005 
        nNewItemNum = 8 
    elseif ( rnum == 27 )  then
        nNewItemID = 4020006 
        nNewItemNum = 8 
    elseif ( rnum == 28 )  then
        nNewItemID = 4020007 
        nNewItemNum = 3 
    elseif ( rnum == 29 )  then
        nNewItemID = 4020008 
        nNewItemNum = 3 
    elseif ( rnum == 30 )  then
        nNewItemID = 1032002 
        nNewItemNum = 1 
    elseif ( rnum == 31 )  then
        nNewItemID = 1032004 
        nNewItemNum = 1 
    elseif ( rnum == 32 )  then
        nNewItemID = 1032005 
        nNewItemNum = 1 
    elseif ( rnum == 33 )  then
        nNewItemID = 1032006 
        nNewItemNum = 1 
    elseif ( rnum == 34 )  then
        nNewItemID = 1032007 
        nNewItemNum = 1 
    elseif ( rnum == 35 )  then
        nNewItemID = 1032009 
        nNewItemNum = 1 
    elseif ( rnum == 36 )  then
        nNewItemID = 1032010 
        nNewItemNum = 1 
    elseif ( rnum == 37 )  then
        nNewItemID = 1002026 
        nNewItemNum = 1 
    elseif ( rnum == 38 )  then
        nNewItemID = 1002089 
        nNewItemNum = 1 
    elseif ( rnum == 39 )  then
        nNewItemID = 1002090 
        nNewItemNum = 1 
    end
    if(inventory->exchange(0, nNewItemID, nNewItemNum) ~= 0) then
        self->say("½Ð½T«O±zªº­I¥]ÁÙ¦³¨¬°÷ªºÄæ¦ìªÅ¶¡¡C")
    else
        target->transferField(103000805, "")
    end
    return
end

if(quest->getVar("stage") == "clear") then party1_reward() return end
if(target->isPartyBoss() == 1) then
    if(fieldID == 103000800) then
        party1_stage1()
    elseif(fieldID == 103000801) then
        party1_stage2()
    elseif(fieldID == 103000802) then
        party1_stage3()
    elseif(fieldID == 103000803) then
        party1_stage4()
    elseif(fieldID == 103000804) then
        party1_stage5()
    end
else
    if(fieldID == 103000800) then
        party1_personal()
    else party1_help() end
end