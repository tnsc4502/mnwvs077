
require "./DataSrv/Script/sysDef"

--Å]ªk´ËªL¥_³¡ÂàÂ¾±Ð©x¹ï¸Ü
function change_magician()
{
    if(math.floor(target->getJob() / 100) != 2 
        or target->getLevel() < 30
        or qr->get(7500) != "s")
    {
        self->say("¹ïªk®vÂ¾·~¦³¿³½ìªº¸ÜÀ³¸Ó¥hÅ]ªk´ËªL§ä#bº~´µ#k...");
        return;
    }
    
    if(inventory->itemCount(4031009) >= 1)
    {
        nBlack = inventory->itemCount(4031013);
        if(nBlack > 0)
        {
            self->sayNext("¤£¤p¤ß¦b¸Ì­±¦º±¼¤F¶Ü¡H¨SÃö«Y¡A§Ú¥i¥H¦A§â§A°e¦^¥h°V½m¦aÂI¡A¦ý¬O½Ð°È¥²¤p¤ß¡A¸Ì­±ªº©Çª«¥i¤£¹³¥L­Ì¥~ªí¯ë®z¤p...");
            self->sayNext("¦b¨º¤§«e¡A§A¨­¤Wªº#b¶Â¦â¯]¤l#k¥²¶·¥æ¥X¨Ó¡Aµ¥§A¶i¤J¦a¹Ï­«·s¶}©l«á¡A³Â·Ð¦A¦¸»`¶°#r30#kÁû#b¶Â¦â¯]¤l#kµ¹§Ú¡C");
            inventory->exchange(0, 4031013, -nBlack);
        }
        else
        {
            self->sayNext("¶â...³o½T¹ê¬O¨Ó¦Û#bº~´µ#kªº«H«Ê...©Ò¥H§A¬O¨Ó³o¸Ì±µ¨ü¬D¾Ô¨Ã¥B¥´ºâ¦¨¬°ªk®v¤GÂàªº«_ÀIŽÍ¶Ü¡H¦n¡A¨º§Ú·|¸ò§A¤¶²Ð³o­Ó¥ô°Èªº¨Æ±¡¡A§O¤Ó¾á¤ß¡A³o¨Ã¨S¦³³o»ò§xÃø¡C");
            self->sayNext("¥ô°È¶}©l«á§Ú·|§â§A°e¶i¤@­ÓÁôÂÃ¦a¹Ï¡C§A·|¦b¨ºÃä¬Ý¨ì¦UºØ±`¨£ªº©Çª«¡CÁöµM¬Ýþ¨Ó»P§A¦b¥~ÀY©Ò¨£¨ìªº©Çª««D±`¬Û¦ü¡A¦ý¥L­Ì¨Ã¤£§¹¥þ¬Û¦P¡C¥L­Ì¨Ã¤£·|´£¤É§Aªº¸gÅç­È¤]¤£·|µ¹§A±j¤jªº¸Ë³Æª««~¡C");
            self->sayNext("¦ý¬O¦b¸Ì­±§A¥i¥HÂÇ¥Ñ¥´±Ñ©Çª«»`¶°¨ì¤@ºØ¥s°µ#b¶Â¦â¯]¤l#kªº¹D¨ã¡A³o¬O¤@ºØ¥Ñ¯S®í§÷½è¥´³yªºª««~¡C¦¬¶°#r30#kÁû#b¶Â¦â¯]¤l#k¨Ã¥B§iª¾¸Ì­±ªº±Ð©x¡A¦p¦¹¤@¨Ó§A´N¥i¥H³q¹L¦ÒÅç¡C");
            if(self->askYesNo("¤@¥¹§A¶i¤J¸Ì­±°£«D§¹¦¨¥ô°È§_«hmµLªkÂ÷¶}¡C­Y¤£©¯¦º±¼¡A±N·|³Q¦©°£¸gÅç­È¡A©Ò¥H³Ì¦n¤p¤ßÂÔ·V¤@ÂI¡C©Ò¥H§A½T©w­n¶i¥h¤F¶Ü¡H") != 1)
                return;
            self->say("¦n¡A¨º§Ú´N§â§A°e¶i¥h¡A¯¬§A¦n¹B¡C");
        }
        target->transferField(108000200 + random(0, 2), "");
    }
    else
        self->say("¦b±µ¨ü¦ÒÅç¤§«e¥²¶·Àò±o#bº~´µ#kªº¦P·N...")    
}

--°V½m³õ¤º³¡ÂàÂ¾±Ð©x¹ï¸Ü
function inside_magician()
{
    nBlack = inventory->itemCount(4031013);
    if(nBlack != 30)
    {
        self->say("ÁÙ¨S»`¶°¨ì#r30#kÁû#b¶Â¦â¯]¤l#k¶Ü¡H¥ô°È§¹¦¨¤§«e¬OµLªkÂ÷¶}ªº...");
        return;
    }
    self->sayNext("«z¡I§A»`¶°¨ì¤F#r30#kÁû#b¶Â¦â¯]¤l#k¡I¡I¨S·Q¨ì¹ï§A¨Ó»¡³o»ò®e©ö...¤Ó¼F®`¤F¡C¦nªº¡A§A¤w¸g³q¹L¤F³o­Ó¦ÒÅç¡A¨Ã¥B§Ú·|µ¹§A#b#t4031012##k¡C§â¥L±a¦^¥hµ¹Å]ªk´ËªLªº#bº~´µ#k¡C")
    if(inventory->exchange(0, 4031013, -nBlack, 4031012, 1) != 0)
    {
        self->say("¶â...§A½T©w¦³¨¬°÷¼Æ¶qªº#b¶Â¦â¯]¤l¶Ü¡H¥t¥~¡A½Ð½T«O¨ä¥LÄæ¦ì¦³¨¬°÷ªºªÅ¶¡¥i¥H¦s©ñ#b#t4031012##k¡C");
        return;
    }
    inventory->exchange(0, 4031009, -1);
    target->transferField(101020000, "");
}