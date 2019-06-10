
require "./DataSrv/Script/sysDef"

if(self->askYesNo("只要搭乘這艘船，你就可以前往更大的世界冒險。只要 #e150楓幣#n，我會帶你到#b維多利亞港#k。但是，一旦離開這個地方，就永遠不能回來了。如何？你想離開這裡並前往維多利亞港嗎？") == 1) 
{
    if(inventory->itemCount(4031801) != 0) 
    {
        self->sayNext("好的，麻煩給我150楓幣...咦，那是什麼？是#b路卡斯的推薦信#k嗎？嘿！你應該早點跟我的。沒想到我有這個榮幸可以看到路卡斯推薦來的人，想必你一定有很大的潛力可以成為一位偉大的冒險家！我不應該跟你收取這筆費用的。");
        self->sayNext("既然是路卡斯推薦你來的，那麼就免費載你一成吧，坐穩了！我們現在就立刻出發前往維多利亞港！");
        inventory->exchange(0, 4031801, -1);
        target->transferField(104000000, "");
    }
    else
    {
        self->sayNext("想前往更繁榮的世界嗎？那麼給我#e150 楓幣#n我就可以帶你離開...");
        if(target->getLevel() < 7)
            self->sayNext("讓我看看...我認為你還不夠資格。你必須至少7級才能夠前往維多利亞港。");
        else
        {
            if(inventory->exchange(-150) == 1)
                self->sayNext("什麼？沒有錢卻想搭我的船？...");
            else
            {
                self->sayNext("好的！#e150 楓幣#n我已經收到了！那我們出發吧，前往維多利亞港！");
                target->transferField(104000000, "");
            }
        }
    }
}
else
    self->say("看來您還有尚未完成的事情，等您決定好在與我談話，切記，一旦離開這裡就再也無法回來了！")