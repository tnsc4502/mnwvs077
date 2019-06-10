
require "./DataSrv/Script/sysDef"

function start() 
{
    if(qr->getState(questID) != 0)
        return;

    count = inventory->itemCount(2010007);
    self->say(count);
    self->sayNext("¶Ù¡A¦³¨Æ¶Ü¡H³á¡A§Ú¬OÃ¹³Ç¡A¥i¥H±Ð§A¤@¨Ç¦³¥Îªºª¾ÃÑ¡C");
    self->sayNext("§A»¡¬°¤°»ò§Ú·|¦b³o¶Ü¡H«¢«¢«¢¡I\r§Ú­t³d±Ð¾É¨º¨Ç­è¶i¤J·¬¤§¨¦¥@¬Éªº«_ÀIŽÍ­Ì¡C");
    if(self->askYesNo("©Ò¥H.....Åý§Ú­Ì¨ÓÂI¦³½ìªº§a~") == 1) 
    {
        target->incHP(-(target->getHP() / 2));
        if(inventory->itemCount(2010007) == 0)
            inventory->exchange(0, 2010007, 1);
        self->sayNext("À~¨ì¤F¶Ü¡H¦pªG¦å¶qÅÜ¦¨0¡A§A±N·|¹J¨ì¤j³Â·Ð¡C§Úµ¥µ¥·|µ¹§A¤@Áû#rÃ¹³ÇªºÄ«ªG#k¡C½Ð°È¥²¦¬¤U¥¦¡C¨Ï¥Î¥¦«á§A·|ÅÜ±o§ó¦³¬¡¤O¡C¥´¶}®ø¯ÓÄæ¨ÃÂùÀ»Ä«ªG¡C«ÜÂ²³æªº¡A¥u­n«ö¤@¤UÁä½Lªº#bI#k´N¯à¥´¶}®ø¯ÓÄæ¡C");
        self->sayNext("«ç»ò¡H§AÁÙ¨S§â§Úµ¹§AªºÄ«ªG¦Y±¼¡I¡H¦Y§¹§AªºHP´N·|¤W¤É¤F¡C½Ð±N¦å¶q¦^ÂÐ¨ì100%¦A¨Ó§ä§Ú§a¡C");
        qr->setState(questID, 1);
    } 
    else
        self->say("³o¥i¬O½ñ¥X«_ÀIªº²Ä¤@¨B¡Iµ¥§A·Q²M·¡¦A»P§Ú½Í¸Ü§a¡C");
}

function complete() 
{
    if(qr->getState(questID) ~= 1)
        return;

    if(inventory->itemCount(2010007) ~= 0)
    {
        self->sayNext("«ç»ò¡H§AÁÙ¨S§â§Úµ¹§AªºÄ«ªG¦Y±¼¡I¡H¦Y§¹§AªºHP´N·|¤W¤É¤F¡C½Ð±N¦å¶q¦^ÂÐ¨ì100%¦A¨Ó§ä§Ú§a¡C");
        return;
    }
    self->sayNext("¬O¤£¬O«ÜÂ²³æ¡H§A¥i¥H¦b¥k°¼ªºÄæ¦ì³]©w#b¼öÁä#k¡CÅ¥¤£À´¹ï§a¡H¨C¹j¤@¬q®É¶¡¡A¦å¶q´N·|ž©´_¤F¡CÁöµM«Üªá®É¶¡¡A¦ý¦n¦n¹B¥Îªº¸Ü¥i¥HÀ°§U¤£¤Öªº¡C");
    self->sayNext("¦n¤F¡I§Ú¤w¸g±Ð§A«Ü¦h¤F¡A¸Óµ¹§AÂ§ª«¤F¡C³o¬O¤@¨Ç¥¼¨Ó¯àÀ°§U§A¦b·¬¤§¨¦¥@¬É¦s¬¡ªº¤@¨Çª««~¡A½Ð¦bºò«æªº®É­Ô¨Ï¥Î¥¦­Ì¡C");
    self->sayNext("¶â¡A§Ú¯à±Ð§Aªº´N¥u¦³³o¨Ç¤F¡AÁöµM«Ü¿ò¾Ñ¡A¦ý¬O§Ú­Ì¯uªº¸Ó»¡¦A¨£¤F¡C¦n¦n·ÓÅU¦Û¤v§a¡C\r\n\r\n#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2010000# 3 #t2010000#\r\n#v2010009# 3 #t2010009#\r\n\r\n#fUI/UIWindow.img/QuestIcon/8/0# 10 exp");

    target->incEXP(10);
    inventory->exchange(0, 2010000, 3);
    inventory->exchange(0, 2010009, 3);
    qr->setState(questID, 2);
}