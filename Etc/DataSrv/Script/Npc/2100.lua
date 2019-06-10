
require "./DataSrv/Script/sysDef"

field = self->field();
if(field and field->getID() == 0) 
{
    if(self->askYesNo("Åwªï¨Ó¨ì·¬¤§¨¦¥@¬É¡I³o¸Ìªº°V½m©Ò¬O¬°¤F­nÀ°§Uªì¤ßŽÍ¡ã§Ú·|«ØÄ³±z§Q¥Î°V½m©Ò¨Ó¼ô±x¹CÀ¸¡A·íµM±z¤]¥i¥Hª½±µ²¤¹L¨Ã¶}©l«_ÀI¡A¬O§_­n¶i¤J°V½m³õ©O¡H") == 1)
    {
        self->say("§Ú±N°e±z¶i¤J¡A¦ý¬O¦b¨º¤§«eÁÙ¬O­n´£¿ô±z¡A½Ð°È¥²¿í´`«ü¾É­ûªº«ü¥Ü¡A¯¬±z¦n¹B¡ã");
        target->transferField(1, "");
    }
    else
    {
        if(self->askYesNo("±z¬O§_¯uªº­nª½±µ¶}©l®Èµ{©O¡H") == 1) 
        {
            self->say("¬Ý¨Ó±z¤w¸g¤U©w¨M¤ß¡A¨º§Ú±Nª½±µ±N±z°eÂ÷°V½m©Ò¡A¯¬±z¦n¹B¡ã");
            target->transferField(40000, "");
        }
        else
            self->say("¬Ý¨Ó±z¦n¨S¦³·Ç³Æ¦n¡Aµ¥±z·Ç³Æ¦n«áÀH®É¥i¥H¸ò§Ú»¡¡I");
    }
}
else
{
    self->sayNext("³o¸Ì±N·|¶}©l°V½m½Òµ{¡A¦P®É±z¤]¦³¾÷·|¥i¥HÅéÅç¦U¦¡ªºÂ¾·~¡C");
    self->sayNext("·í±z§V¤O¨ì¹F¤F¤@©wªºµ¥¯Å«á¡A´N¥i¥H¿ï¾ÜÂ¾·~¡A¦b¤}½b¤â§ø¥i¥HÂàÂ¾¬°¤}½b¤â¡AÅ]ªk´ËªL¥i¥HÂàÂ¾¬°ªk®v¡A«i¤h¤§§ø¥i¥HÂàÂ¾¬°¼C¤h¡A¼Z¸¨«°¥««h¥i¥HÂàÂ¾¬°µs¸é¡I");
}