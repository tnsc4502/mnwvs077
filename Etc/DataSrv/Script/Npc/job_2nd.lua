require "./DataSrv/Script/sysDef"

--勇士之村西部轉職教官對話
function change_warrior()
{
    if(math.floor(target->getJob() / 100) != 1
        or target->getLevel() < 30
        or qr->get(7500) != "s")
    {
        self->say("對劍士職業有興趣的話應該去勇士之村找#b武術教練#k...");
        return;
    }
    
    if(inventory->itemCount(4031008) >= 1)
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
            self->sayNext("嗯...這確實是來自#b武術教練#k的信封...所以你是來這裡接受挑戰並且打算成為劍士二轉的冒險者嗎？好，那我會跟你介紹這個任務的事情，別太擔心，這並沒有這麼困難。");
            self->sayNext("任務開始後我會把你送進一個隱藏地圖。你會在那邊看到各種常見的怪物。雖然看起來與你在外頭所見到的怪物非常相似，但他們並不完全相同。他們並不會提升你的經驗值也不會給你強大的裝備物品。");
            self->sayNext("但是在裡面你可以藉由打敗怪物蒐集到一種叫做#b黑色珠子#k的道具，這是一種由特殊材質打造的物品。收集#r30#k顆#b黑色珠子#k並且告知裡面的教官，如此一來你就可以通過考驗。");
            if(self->askYesNo("一旦你進入裡面除非完成任務否則都無法離開。若不幸死掉，將會被扣除經驗值，所以最好小心謹慎一點。所以你確定要進去了嗎？") != 1)
                return;
            self->say("好，那我就把你送進去，祝你好運。");
        }
        target->transferField(108000300 + random(0, 2), "");
    }
    else
        self->say("在接受考驗之前必須獲得#b武術教練#k的同意...")    
}

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
            self->sayNext("嗯...這確實是來自#b漢斯#k的信封...所以你是來這裡接受挑戰並且打算成為法師二轉的冒險者嗎？好，那我會跟你介紹這個任務的事情，別太擔心，這並沒有這麼困難。");
            self->sayNext("任務開始後我會把你送進一個隱藏地圖。你會在那邊看到各種常見的怪物。雖然看起來與你在外頭所見到的怪物非常相似，但他們並不完全相同。他們並不會提升你的經驗值也不會給你強大的裝備物品。");
            self->sayNext("但是在裡面你可以藉由打敗怪物蒐集到一種叫做#b黑色珠子#k的道具，這是一種由特殊材質打造的物品。收集#r30#k顆#b黑色珠子#k並且告知裡面的教官，如此一來你就可以通過考驗。");
            if(self->askYesNo("一旦你進入裡面除非完成任務否則都無法離開。若不幸死掉，將會被扣除經驗值，所以最好小心謹慎一點。所以你確定要進去了嗎？") != 1)
                return;
            self->say("好，那我就把你送進去，祝你好運。");
        }
        target->transferField(108000200 + random(0, 2), "");
    }
    else
        self->say("在接受考驗之前必須獲得#b漢斯#k的同意...")    
}

--迷宮通道轉職教官對話
function change_archer()
{
    if(math.floor(target->getJob() / 100) != 3 
        or target->getLevel() < 30
        or qr->get(7500) != "s")
    {
        self->say("對弓箭手職業有興趣的話應該去弓箭手村找#b赫麗娜#k...");
        return;
    }
    
    if(inventory->itemCount(4031010) >= 1)
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
            self->sayNext("嗯...這確實是來自#b赫麗娜#k的信封...所以你是來這裡接受挑戰並且打算成為弓箭手二轉的冒險者嗎？好，那我會跟你介紹這個任務的事情，別太擔心，這並沒有這麼困難。");
            self->sayNext("任務開始後我會把你送進一個隱藏地圖。你會在那邊看到各種常見的怪物。雖然看起來與你在外頭所見到的怪物非常相似，但他們並不完全相同。他們並不會提升你的經驗值也不會給你強大的裝備物品。");
            self->sayNext("但是在裡面你可以藉由打敗怪物蒐集到一種叫做#b黑色珠子#k的道具，這是一種由特殊材質打造的物品。收集#r30#k顆#b黑色珠子#k並且告知裡面的教官，如此一來你就可以通過考驗。");
            if(self->askYesNo("一旦你進入裡面除非完成任務否則都無法離開。若不幸死掉，將會被扣除經驗值，所以最好小心謹慎一點。所以你確定要進去了嗎？") != 1)
                return;
            self->say("好，那我就把你送進去，祝你好運。");
        }
        target->transferField(108000100 + random(0, 2), "");
    }
    else
        self->say("在接受考驗之前必須獲得#b赫麗娜#k的同意...")    
}

--迷宮通道轉職教官對話
function change_thief()
{
    if(math.floor(target->getJob() / 100) != 4 
        or target->getLevel() < 30
        or qr->get(7500) != "s")
    {
        self->say("對弓箭手職業有興趣的話應該去墮落城市找#b達克魯#k...");
        return;
    }
    
    if(inventory->itemCount(4031011) >= 1)
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
            self->sayNext("嗯...這確實是來自#b達克魯#k的信封...所以你是來這裡接受挑戰並且打算成為盜賊二轉的冒險者嗎？好，那我會跟你介紹這個任務的事情，別太擔心，這並沒有這麼困難。");
            self->sayNext("任務開始後我會把你送進一個隱藏地圖。你會在那邊看到各種常見的怪物。雖然看起來與你在外頭所見到的怪物非常相似，但他們並不完全相同。他們並不會提升你的經驗值也不會給你強大的裝備物品。");
            self->sayNext("但是在裡面你可以藉由打敗怪物蒐集到一種叫做#b黑色珠子#k的道具，這是一種由特殊材質打造的物品。收集#r30#k顆#b黑色珠子#k並且告知裡面的教官，如此一來你就可以通過考驗。");
            if(self->askYesNo("一旦你進入裡面除非完成任務否則都無法離開。若不幸死掉，將會被扣除經驗值，所以最好小心謹慎一點。所以你確定要進去了嗎？") != 1)
                return;
            self->say("好，那我就把你送進去，祝你好運。");
        }
        target->transferField(108000400 + random(0, 2), "");
    }
    else
        self->say("在接受考驗之前必須獲得#b達克魯#k的同意...")    
}
----------------INSIDE----------------

--訓練場內部轉職教官對話
function inside_warrior()
{
    nBlack = inventory->itemCount(4031013);
    if(nBlack != 30)
    {
        self->say("還沒蒐集到#r30#k顆#b黑色珠子#k嗎？任務完成之前是無法離開的...");
        return;
    }
    self->sayNext("哇！你蒐集到了#r30#k顆#b黑色珠子#k！！沒想到對你來說這麼容易...太厲害了。好的，你已經通過了這個考驗，並且我會給你#b#t4031012##k。把他帶回去給勇士之村的#b武術教練#k。")
    if(inventory->exchange(0, 4031013, -nBlack, 4031012, 1) != 0)
    {
        self->say("嗯...你確定有足夠數量的#b黑色珠子嗎#k？另外，請確保其他欄位有足夠的空間可以存放#b#t4031012##k。");
        return;
    }
    inventory->exchange(0, 4031008, -1);
    target->transferField(102020300, "");
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
        self->say("嗯...你確定有足夠數量的#b黑色珠子嗎#k？另外，請確保其他欄位有足夠的空間可以存放#b#t4031012##k。");
        return;
    }
    inventory->exchange(0, 4031009, -1);
    target->transferField(101020000, "");
}

--訓練場內部轉職教官對話
function inside_archer()
{
    nBlack = inventory->itemCount(4031013);
    if(nBlack != 30)
    {
        self->say("還沒蒐集到#r30#k顆#b黑色珠子#k嗎？任務完成之前是無法離開的...");
        return;
    }
    self->sayNext("哇！你蒐集到了#r30#k顆#b黑色珠子#k！！沒想到對你來說這麼容易...太厲害了。好的，你已經通過了這個考驗，並且我會給你#b#t4031012##k。把他帶回去給弓箭手村的#b赫麗娜#k。")
    if(inventory->exchange(0, 4031013, -nBlack, 4031012, 1) != 0)
    {
        self->say("嗯...你確定有足夠數量的#b黑色珠子嗎#k？另外，請確保其他欄位有足夠的空間可以存放#b#t4031012##k。");
        return;
    }
    inventory->exchange(0, 4031010, -1);
    target->transferField(100000000, "");
}

--訓練場內部轉職教官對話
function inside_thief()
{
    nBlack = inventory->itemCount(4031013);
    if(nBlack != 30)
    {
        self->say("還沒蒐集到#r30#k顆#b黑色珠子#k嗎？任務完成之前是無法離開的...");
        return;
    }
    self->sayNext("哇！你蒐集到了#r30#k顆#b黑色珠子#k！！沒想到對你來說這麼容易...太厲害了。好的，你已經通過了這個考驗，並且我會給你#b#t4031012##k。把他帶回去給墮落城市的#b達克魯#k。")
    if(inventory->exchange(0, 4031013, -nBlack, 4031012, 1) != 0)
    {
        self->say("嗯...你確定有足夠數量的#b黑色珠子嗎#k？另外，請確保其他欄位有足夠的空間可以存放#b#t4031012##k。");
        return;
    }
    inventory->exchange(0, 4031011, -1);
    target->transferField(103000000, "");
}

--訓練場內部轉職教官對話
function inside_pirate()
{
    nItemID = 0;
    if(qr->getState(2191) == 1) 
        nItemID = 4031856;
    else if(qr->getState(2192) == 1)
        nItemID = 4031857;

    nBlack = inventory->itemCount(nItemID);
    if(nBlack != 15)
    {
        self->say("還沒蒐集到#r15#k個#b#t" .. nItemID .. "##k嗎？任務完成之前是無法離開的...");
        return;
    }
    self->sayNext("喔喔～你已經把#r15#k個#b#t" .. nItemID .. "##k都蒐集齊全啦！太厲害！這應該花了你不少力氣吧！好～我們到鯨魚號再慢慢聊吧！");
    self->sayNext("這些水晶既然只能在這裡使用，那我把它們再拿回去囉！");

    target->transferField(120000101, "");
}