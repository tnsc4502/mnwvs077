require "./DataSrv/Script/sysDef"

if(inventory->itemCount(4031450) > 0)
{
    target->transferField(921100100, "");
    return;
}

self->sayNext("嘿，你看起來似乎想要探索更深入的地方？在那裡面有很多強大的怪物會包圍著你，不管怎樣，你一定要小心。很久以前，與我同村的男人想要進去消滅那些威脅村子的怪物，可是就再也沒有回來了...");
sayStr = "我認為你還是趕快改變心意，但是如果你堅決要前進的話...我得看看你是不是有這個資格，我不希望有人白白進去送死，";
if(target->getLevel() >= 50)
    sayStr = sayStr .. "嗯...你看起來應該滿有實力的，好吧，你確定要進入？";
else
    sayStr = sayStr .. "嗯...等到50級之後再考慮看看也不遲？你確定要進入？";

if(self->askYesNo(sayStr) == 1)
    target->transferField(211040300, "under00");
else
    self->say("儘管你的等級這麼高，還是很難鼓起勇氣前往對吧？沒關係，當你改變心意，再來找我，我的職責就是保護這裡");