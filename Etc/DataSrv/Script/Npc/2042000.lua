require "./DataSrv/Script/sysDef"

ret = self->askMenu("#L0#MC100#l\r\n#L1#MC200#l\r\n#L2#MC300#l\r\n#L3#MC400#l\r\n#L4#MC500#l\r\n");

fieldSet = FieldSet.get("MC100");
if(fieldSet) {
    ret = fieldSet->enter(userID);

::ENTER_RESULT::
    if (ret == 1)
        self->say("­n¬D¾Ô¶W¯Åºñ¤ôÆF²Õ¶¤¥ô°È¡A³æ¾a¦Û¤v¤@­Ó¤H¬O¤£¥i¯àªº¡A©Ò¥H½Ð¥ý¥[¤J¶¤¥î«á¦A»P§Ú½Í¸Ü¡C");
    elseif (ret == 2)
        self->say("½Ð¶¤ªø»P§Ú½Í¸Ü¡C");
    elseif (ret == 3 or ret == 4) 
        self->say("±ý¶i¦æ¶W¯Åºñ¤ôÆF²Õ¶¤¥ô°È¡A±zªº¶¤¥î¤H¼Æ¦Ü¤Ö#r4¤H#k¡Aµ¥¯Å¬É©ó#r21¦Ü30¯Å#k¤§¶¡¡A¨Ã¥B¥Ñ¶¤ªø»P§Ú¥æ½Í¡I");
    elseif (ret == 5) {
        ret = fieldSet->join(userID);
        if(ret == 5)
            self->say("¤w¸g¦³¶¤¥î¥Ø«e¥¿¦b¬D¾Ô²Õ¶¤¥ô°È¡A½Ð§ó´«ÀW¹D©ÎŽÍµ¥«Ý¥¿¦b¶i¦æªº¶¤¥îµ²§ô¡C");
        else
            goto ENTER_RESULT;
    }
}