require "./DataSrv/Script/sysDef"

fieldID = self->field()->getID();

--Default settings
cost = 45000;
targetField = 211040200;

if(fieldID == 540000000) { targetField = 541020000; cost = 30000; }
else if(fieldID == 240000000) { targetField = 240030000; cost = 55000; }
else if(fieldID == 220000000) { targetField = 220050300; cost = 45000; }
else if(fieldID == 211000000) { targetField = 211040200; cost = 45000; }

self->sayNext("你好，我是#b#p" .. templateID .. "##k，我可以帶你到各種危險的地方，你可以從#b#m" .. self->field()->getID() .. "##k前往#b#m" .. targetField .. "##k，並且只需要#b" .. cost .. "#k楓幣！");
if(self->askYesNo("如何？你有打算前往#b#m" .. targetField .. "##k嗎？請注意，危險地區的怪物特別強悍，除非做好萬全準備，否則#r不建議前往#k。") == 1)
{
    if(inventory->exchange(-cost) != 0)
        self->say("您沒有足夠的楓幣，一共需要#b" .. cost .. "#k楓幣。");
    else
        target->transferField(targetField, "");
}