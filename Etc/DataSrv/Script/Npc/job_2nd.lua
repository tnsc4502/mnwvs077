
require "./DataSrv/Script/sysDef"

--魔法森林北部轉職教官對話
function change_magician()
{
    if(math.floor(target->getJob() / 100) != 2 
        or target->getLevel() < 30
        or qr->get(7500) != "s")
    {
        self->say("對法師職業有興趣的話應該去魔法森林找#b漢斯#k...");
        return;
    }
    
    if(inventory->itemCount(4031009) >= 1)
    {
        nBlack = inventory->itemCount(4031013);
        if(nBlack > 0)
        {
            self->sayNext("不小心在裡面死掉了嗎？沒關係，我可以再把你送回去訓練地點，但是請務必小心，裡面的怪物可不像他們外表般弱小...");
            self->sayNext("在那之前，你身上的#b黑色珠子#k必須交出來，等你進入地圖重新開始後，麻煩再次蒐集#r30#k顆#b黑色珠子#k給我。");
            inventory->exchange(0, 4031013, -nBlack);
        }
        else
        {
            self->sayNext("嗯...這確實是來自#b漢斯#k的信封...所以你是來這裡接受挑戰並且打算成為法師二轉的冒險�芘隉H好，那我會跟你介紹這個任務的事情，別太擔心，這並沒有這麼困難。");
            self->sayNext("任務開始後我會把你送進一個隱藏地圖。你會在那邊看到各種常見的怪物。雖然看��來與你在外頭所見到的怪物非常相似，但他們並不完全相同。他們並不會提升你的經驗值也不會給你強大的裝備物品。");
            self->sayNext("但是在裡面你可以藉由打敗怪物蒐集到一種叫做#b黑色珠子#k的道具，這是一種由特殊材質打造的物品。收集#r30#k顆#b黑色珠子#k並且告知裡面的教官，如此一來你就可以通過考驗。");
            if(self->askYesNo("一旦你進入裡面除非完成任務否則�m無法離開。若不幸死掉，將會被扣除經驗值，所以最好小心謹慎一點。所以你確定要進去了嗎？") != 1)
                return;
            self->say("好，那我就把你送進去，祝你好運。");
        }
        target->transferField(108000200 + random(0, 2), "");
    }
    else
        self->say("在接受考驗之前必須獲得#b漢斯#k的同意...")    
}

--訓練場內部轉職教官對話
function inside_magician()
{
    nBlack = inventory->itemCount(4031013);
    if(nBlack != 30)
    {
        self->say("還沒蒐集到#r30#k顆#b黑色珠子#k嗎？任務完成之前是無法離開的...");
        return;
    }
    self->sayNext("哇！你蒐集到了#r30#k顆#b黑色珠子#k！！沒想到對你來說這麼容易...太厲害了。好的，你已經通過了這個考驗，並且我會給你#b#t4031012##k。把他帶回去給魔法森林的#b漢斯#k。")
    if(inventory->exchange(0, 4031013, -nBlack, 4031012, 1) != 0)
    {
        self->say("嗯...你確定有足夠數量的#b黑色珠子嗎？另外，請確保其他欄位有足夠的空間可以存放#b#t4031012##k。");
        return;
    }
    inventory->exchange(0, 4031009, -1);
    target->transferField(101020000, "");
}