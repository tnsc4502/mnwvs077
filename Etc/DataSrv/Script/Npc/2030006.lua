
require "./DataSrv/Script/sysDef"

function wizQuestion(index)
{
    ::INCORRECT_ANSWER::
    if(errorReport == 1)
    {
        self->say("«Ü¿ò¾Ñ¡A§Aµª¿ù¤F¡A¥²¶·­«·s¶}©l¡C");
        return;
    }
    errorReport = 1;

    if(index == 1)
    {
        ans = self->askMenu("°ÝÃD¤@¡G±q·¬¤§¨¦¤@¶}©l¡A¹J¨ìªº²Ä¤@­ÓNPC¬O½Ö¡H#b\r\n#L0# #p2000##l\r\n#L1# #p1010100##l\r\n#L2# #p2101##l\r\n#L3# #p2001##l\r\n#L4# #p2102##l");
        if(ans != 2) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤G¡G¤U¦C¤¤¡A¦b·¬¤§®qµLªk¨£¨ìªº©Çª«¬O­þ¤@­Ó¡H#b\r\n#L0# #o100101##l\r\n#L1# #o1210102##l\r\n#L2# #o130101##l\r\n#L3# #o1210100##l\r\n#L4# #o120100##l");
        if(ans != 3) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤T¡G¤}½b¤â§øªºº¿¨È½Ð¨D§Ú­Ì®³¤°»òª««~µ¹¦o¡A¨Óªv¦n¦oªº¯f¡H#b\r\n#L0# ¬õ¦âÃÄ¤ô #l\r\n#L1# #t4031654# #l\r\n#L2# #t2002011#  #l\r\n#L3# #t2002015# #l\r\n#L4# #t4031006##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¥|¡G·¬¤§¨¦¤U¦CÃÄ«~¤¤¡A­þ²ÕÃÄ«~»P®ÄªG¬O¤£¥¿½T¹ïÀ³Ãö«Yªº¡H#b\r\n#L0# #t2050003# - ¸Ñ°£©Ò¦³²§±`ª¬ºA#l\r\n#L1# #t2020014# - MP 3000 ¦^´_#l\r\n#L2# #t2020004# - HP 400 ¦^´_#l\r\n#L3# #t2020000# - MP 200 ¦^´_#l\r\n#L4# #t2000003# - MP 100 ¦^´_#l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤­¡G¤U¦C¤¤¡A¦bºû¦h§Q¨È®qµLªk¨£¨ìªº©Çª«¬O­þ¤@­Ó¡H#b\r\n#L0# #o2110200##l\r\n#L1# #o2230100##l\r\n#L2# #o5130100##l\r\n#L3# #o2230101##l\r\n#L4# #o3000000##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
    }
    else if(index == 2)
    {
        ans = self->askMenu("°ÝÃD¤@¡G¤U¦C¤¤¡A¦b¦ã¯Ç´µ®qªºªüº¸ªk¤p¶¤¤¤¨S¦³ªº¤Hª«¬O½Ö¡H#b\r\n#L0# ¥Ö¯S¤¤¤h #l\r\n#L1# #p2010000##l\r\n#L2# #p2020003##l\r\n#L3# #p2030002##l\r\n#L4# #p2030001##l");
        if(ans != 0) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤G¡G¤U¦C¤¤¡A³ê¿ô³Á¦NªºÂÂ¾Ô¼C¤£»Ý­nªº§÷®Æ¬O¤°»ò¡H#b\r\n#L0# #t4003002##l\r\n#L1# #t4021009##l\r\n#L2# #t4001006##l\r\n#L3# #t4003003##l\r\n#L4# #t4001005##l");
        if(ans != 3) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤T¡G¤U¦C¤¤¡A¦bºû¦h§Q¨È®qªº¼Z¸¨«°¥«µLªk¨£¨ìªºNPC¬O½Ö¡H#b\r\n#L0# #p1052002##l\r\n#L1# #p1052102##l\r\n#L2# #p1052012##l\r\n#L3# #p1052100##l\r\n#L4# #p1040000##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¥|¡G¤U¦C¤¤¡A­þ²Õ©Çª«»P¥´­Ë¥¦©Ò¯à±o¨ìªº¾Ô§Q«~¤£§k¦Xªº¬O­þ¤@­Ó¡H#b\r\n#L0# #o3230200# - #t4000059##l\r\n#L1# #o4230105# - ­¹¤Hªáªº¸­¤l#l\r\n#L2# #o6130101# - #t4000040##l\r\n#L3# #o6130103# - #t4000050##l\r\n#L4# #o3210800# - #t4000029##l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤­¡G¤U¦C¤¤¡A·|­¸ªº©Çª«¬O­þ¤@­Ó¡H#b\r\n#L0# #o5130104##l\r\n#L1# #o4230105##l\r\n#L2# #o4230103##l\r\n#L3# #o4130101##l\r\n#L4# #o5300100##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
    }
    else if(index == 3)
    {
        ans = self->askMenu("°ÝÃD¤@¡G¦bºû¦h§Q¨È®qªº¤}½b¤â§ø¬Ý¤£¨£ªºNPC¬O½Ö¡H\r\n#b#L0# #p1012101##l\r\n#L1# #p1002001##l\r\n#L2# #p1010100##l\r\n#L3# #p1012100##l\r\n#L4# #p1012102##l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤G¡G·¬¤§¨¦¤¤¡A±qµ¥¯Å1¨ìµ¥¯Å2»Ý­n¦h¤Ö¸gÅç­È¡H#b\r\n#L0# 10#l\r\n#L1# 15#l\r\n#L2# 20#l\r\n#L3# 25#l\r\n#L4# 30#l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤T¡G¥H¤U­n¨Dµ¥¯Å³Ì°ªªº¥ô°È¬O¡H#b\r\n#L0# ³Á¦NªºÂÂ¾Ô¼C #l\r\n#L1# ¦u½Ã§L¾|§Jªº¨M¤ß #l\r\n#L2# ´M§ä¥j¥N¤§®Ñ #l\r\n#L3# ¨È³Í´µ¯S©M¶Â·t¤ô´¹ #l\r\n#L4# ªüº¸ªk³¡¶¤Ápµ¸ºô #l");
        if(ans != 3) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¥|¡G¤U¦C¤¤¡A­þ­ÓÂ¾·~¤£¬O2Âà¤¤¥X²{ªºÂ¾·~¡H#b\r\n#L0# ¨g¾Ô¤h#l\r\n#L1# ¦B¹p§Å®v#l\r\n#L2# ¹¬«Q#l\r\n#L3# Å]¾É¤h#l\r\n#L4# Ây¤H#l");
        if(ans != 3) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤­¡G¤U¦C¤¤¡A¥i¥H­«½Æ°õ¦æªº¥ô°È¬O¡H#b\r\n#L0# À°§U¤×®R#l\r\n#L1# ¬¼ÂyÛ£Û£Ä_¨©#l\r\n#L2# ­×²z­©ªøªº®a#l\r\n#L3# ¦ã·Åªº¬Á¼þ¾c#l\r\n#L4# ªø¦Ñªº¤¶²Ð#l");
        if(ans != 3) { goto INCORRECT_ANSWER; }
    }
    else if(index == 4)
    {
        ans = self->askMenu("°ÝÃD¤@¡G¤U¦C¤¤¡A­þ²Õ©Çª«»P¥´­Ë¥¦©Ò¯à±o¨ìªº¾Ô§Q«~§k¦Xªº¬O­þ¤@­Ó¡H#b\r\n#L0# #o3210100# - ªÎªÎ½v±a#l\r\n#L1# #o2300100# - #t4000042##l\r\n#L2# #o1210100# - ¹àÄ_´ß#l\r\n#L3# #o4230100# - Às¤§¦å#l\r\n#L4# #o2230101# - Û£Û£ÌU¤ú#l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤G¡G¤U¦C¤¤¡A¦bºû¦h§Q¨È®qªº«i¤h¤§§øµLªk¨£¨ìªºNPC¬O½Ö¡H#b\r\n#L0# #p1021100##l\r\n#L1# #p1022100##l\r\n#L2# #p1022002##l\r\n#L3# #p1022003##l\r\n#L4# #p1032002##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤T¡G¦b¼Z¸¨«°¥«¤¤¡AÂ÷®a¥X¨«ªº¤Ö¦~ªü°Ç´µªº¤÷¿Ë¬O½Ö¡H#b\r\n#L0# #p1012005##l\r\n#L1# #p1012002##l\r\n#L2# #p12000##l\r\n#L3# #p20000##l\r\n#L4# #p1012003##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¥|¡G¬°¤F2Âà¦Ó¦¬¶°30­Ó¶Â¯]µ¹ÂàÂ¾±Ð©x¡A±q¥L¨ºùØ±o¨ì¤°»ò¡H#b\r\n#L0# #t4031012##l\r\n#L1# #t4031057##l\r\n#L2# #t4031058##l\r\n#L3# ªø¦Ñªº±ÀÂË«H#l\r\n#L4# «_ÀI®a¾±³¹#l");
        if(ans != 0) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤­¡G®Ú¾Ú¤£¦PÂ¾·~ªº²Ä1¦¸ÂàÂ¾¥²¶·±ø¥ó¡A¥¿½T¥B¥²¶·ªº¬O­þ¤@­Ó¡H#b\r\n#L0# ¼C¤h - ¤O¶q 30+#l\r\n#L1# ªk®v - ´¼¤O 25+#l\r\n#L2# ¤}½b¤â - ±Ó±¶ 25+#l\r\n#L3# µs¸é - ±Ó±¶ 20+#l\r\n#L4# µs¸é - ¤O¶q 20+#l");
        if(ans != 2) { goto INCORRECT_ANSWER; }
    }
    else if(index == 5)
    {
        ans = self->askMenu("°ÝÃD¤@¡G³Q©Çª«§ðÀ»®É¯S§Oªº²§±`ª¬ºA¨S¦³³Q¥¿½T»¡©úªº¬O­þ¤@­Ó¡H\r\n#b#L0# ¶Â·t¡X¡X©R¤¤²v¤U­° #l\r\n#L1# ¶A©G¡X¡XÀò±oªºEXP´î¤Ö #l\r\n#L2# µê®z¡X¡X²¾°Ê³t«×­°§C #l\r\n#L3# «Ê¦L¡X¡XµLªk¨Ï¥Î§Þ¯à #l\r\n#L4# ¤¤¬r¡X¡X½wºC´î¤ÖHP #l");
        if(ans != 2) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤G¡G¦b¦ã¯Ç´µ®q¨S¦³­þ­Ó©Çª«¡H#b\r\n#L0# #o5130103##l\r\n#L1# #o5140000##l\r\n#L2# #o6300000##l\r\n#L3# #o8140000##l\r\n#L4# #o5120000##l");
        if(ans != 0) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤T¡G¤U¦C¤¤¡A¦b¦ã¯Ç´µ®qªº¦B­ì³·°ìµLªk¨£¨ìªºNPC¬O½Ö¡H#b\r\n#L0# #p2020000##l\r\n#L1# #p2020003##l\r\n#L2# #p2012010##l\r\n#L3# #p2020006##l\r\n#L4# #p2020007##l");
        if(ans != 2) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¥|¡G¤U¦C¤£¬O¶i¦æ¦X¦¨¡B·Ò»s¡B»s§@ªºNPC¬O­þ¤@­Ó¡H#b\r\n#L0# #p2010003##l\r\n#L1# #p1022003##l\r\n#L2# #p1032003##l\r\n#L3# #p1032002##l\r\n#L4# #p2020000##l");
        if(ans != 2) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤­¡G·¬¤§¨¦¤U¦CÃÄ«~¤¤¡A­þ²ÕÃÄ«~»P®ÄªG¬O¥¿½T¹ïÀ³Ãö«Yªº¡H#b\r\n#L0# #t2000001# - HP 200 ž©´_#l\r\n#L1# #t2001001# - MP 2000 ž©´_#l\r\n#L2# #t2010004# - MP 100 ž©´_#l\r\n#L3# #t2020001# - HP 300 ž©´_#l\r\n#L4# #t2020003# - HP 400 ž©´_#l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
    }
    else if(index == 6)
    {
        ans = self->askMenu("°ÝÃD¤@¡G¦bºû¦h§Q¨È®qªºÅ]ªk´ËªL¬Ý¤£¨£ªºNPC¬O½Ö¡H#b\r\n#L0# #p1032003##l\r\n#L1# #p1032002##l\r\n#L2# #p1032001##l\r\n#L3# #p1032100##l\r\n#L4# #p1081101##l");
        if(ans != 4) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤G¡G¦b¦ã¯Ç´µ®qªº¤ÑªÅ¤§«°¬Ý¤£¨£ªºNPC¬O½Ö¡H#b\r\n#L0# #p2010000##l\r\n#L1# #p1022100##l\r\n#L2# #p2010003##l\r\n#L3# #p2012004##l\r\n#L4# #p2012005##l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤T¡GºñÛ£Û£¡B¤ì§¯¡BÂÅ¤ôÆF¡B©ò¤ì§¯¡B¤T²´³¹³½¡A­þ­Ó¬Oµ¥¯Å³Ì°ªªº©Çª«¡H#b\r\n#L0# ºñÛ£Û£ #l\r\n#L1# ©ò¤ì§¯ #l\r\n#L2# ÂÅ¤ôÆF #l\r\n#L3# ¤ì§¯ #l\r\n#L4# ¤T²´³¹³½ #l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¥|¡G¤U¦C¤¤¡A¦bºû¦h§Q¨È®q¨S¦³ªº§ø²ø¬O¡H#b\r\n#L0# ©_¤Û§ø #l\r\n#L1# ·¬¸­§ø #l\r\n#L2# «i¤h¤§§ø #l\r\n#L3# ¼Z¸¨«°¥« #l\r\n#L4# Å]ªk´ËªL#l");
        if(ans != 1) { goto INCORRECT_ANSWER; }
        ans = self->askMenu("°ÝÃD¤­¡G»PÃdª«µLÃöªºNPC¬O½Ö¡H#b\r\n#L0# #p1012005##l\r\n#L1# #p1032102##l\r\n#L2# #p1012007##l\r\n#L3# #p1012002##l\r\n#L4# #p1012004##l");
        if(ans != 3) { goto INCORRECT_ANSWER; }
    }
    self->say("®¥³ß§A§¹¦¨¤F©Ò¦³ªº´úÅç...§Aªº´¼¼z¤w¸gÀò±oÀËÅç...½Ð§â³o­Ó¶µÁåÀ¹¦b¨­¤WµM«á¦^¥h¡C");
    if(inventory->exchange(0, 4031058, 1) != 0)
        self->say("¤£·|§a¡H¬Ýþ¨Ó§Aªº¨ä¥LÄæ¦ìªÅ¶¡¤£¨¬...");
}

if(target->getLevel() >= 70 and qr->get(7500) == "end1")
{
    if(inventory->itemCount(4031058) != 0)
        self->say("§AÁÙ¨S§â¶µÁå±a¦^¥hµ¹ªø¦Ñ­Ì¶Ü¡H");
    else if(self->askYesNo("... ... ...\r\n¦pªG§A·Q­n³q¹L´¼¼zªº¦ÒÅç¡A§A¥²¶·¥¿½T¦^µª§Ú©Ò´£¥Xªº°ÝÃD¡A¤£¹L¡A¦b¨º¤§«e¡A§A¥²¶·¥æ¥X#b#t4005004##k§@¬°¥N»ù¡A§A·Ç³Æ¦n¤F¶Ü¡H") == 1)
    {
        if(inventory->exchange(0, 4005004, -1) != 0)
            self->say("§A¨­¤W¨S¦³¥ô¦óªº#b#t4005004##k¡A§ÚµLªk¬°§A¶i¦æ´úÅç...");
        else
        {
            self->say("¨º»ò±µ¤U¨Ó§Ú±N¶}©l´¼¼z´úÅç¡C¦^µª¤U¦C©Ò¦³°ÝÃD¡A¨º§A´N¥i¥H³q¹L¦ÒÅç¡C¦ý¬O¦pªG§A¦^µª¿ù¤F¡A©ÎŽÍ¸Õ¹Ï»¡ÁÀ¡A¨º§A¥²¶·­«·s¨Ó¹L...");
            wizQuestion(random(1, 6));
        }
    }
    else
        self->say("µ¥§A·Ç³Æ¦n®É¡A¦A¨Ó»P§Ú½Í¸Ü¡C");
}