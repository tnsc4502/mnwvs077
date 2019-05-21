require "./DataSrv/Script/sysDef"

fieldSet = FieldSet.get("Party2")
if(fieldSet) then
    ret = fieldSet->enter(userID)
   if (ret == 1) then
        self->say("­n¬D¾Ôª±¨ã«°101²Õ¶¤¥ô°È¡A³æ¾a¦Û¤v¤@­Ó¤H¬O¤£¥i¯àªº¡A©Ò¥H½Ð¥ý¥[¤J¶¤¥î«á¦A»P§Ú½Í¸Ü¡C")
    elseif (ret == 2) then
        self->say("½Ð¶¤ªø»P§Ú½Í¸Ü¡C")
    elseif (ret == 3 or ret == 4) then
        self->say("±ý¶i¦æª±¨ã«°101²Õ¶¤¥ô°È¡A±zªº¶¤¥î¤H¼Æ¦Ü¤Ö#r5¤H#k¡Aµ¥¯Å¬É©ó#r36¦Ü50¯Å#k¤§¶¡¡A¨Ã¥B¥Ñ¶¤ªø»P§Ú¥æ½Í¡I")
    elseif (ret == 5) then
        self->say("¤w¸g¦³¶¤¥î¥Ø«e¥¿¦b¬D¾Ô²Õ¶¤¥ô°È¡A½Ð§ó´«ÀW¹D©ÎŽÍµ¥«Ý¥¿¦b¶i¦æªº¶¤¥îµ²§ô¡C")
    end
end