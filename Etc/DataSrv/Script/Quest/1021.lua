
require "./DataSrv/Script/sysDef"

function start()
    if(qr->getState(questID) ~= 0) then
        return
    end

    count = inventory->itemCount(2010007)
    self->say(count)

    self->sayNext("嗨，有事嗎？喔，我是羅傑，可以教你一些有用的知識。");
    self->sayNext("你說為什麼我會在這嗎？哈哈哈！\r我負責教導那些剛進入楓之谷世界的冒險者們。");
    if(self->askYesNo("所以.....讓我們來點有趣的吧~") == 1) then
        target->incHP(-(target->getHP() / 2))
        if(inventory->itemCount(2010007) == 0) then
            inventory->exchange(0, 2010007, 1)
         end
        self->sayNext("嚇到了嗎？如果血量變成0，你將會遇到大麻煩。我等等會給你一顆#r羅傑的蘋果#k。請務必收下它。使用它後你會變得更有活力。打開消耗欄並雙擊蘋果。很簡單的，只要按一下鍵盤的#bI#k就能打開消耗欄。");
        self->sayNext("怎麼？你還沒把我給你的蘋果吃掉！？吃完你的HP就會上升了。請將血量回覆到100%再來找我吧。")
        qr->setState(questID, 1)
    else
        self->say("這可是踏出冒險的第一步！等你想清楚再與我談話吧。")
    end
end

function complete()
    if(qr->getState(questID) ~= 1) then
        return
    end

    if(inventory->itemCount(2010007) ~= 0) then
        self->sayNext("怎麼？你還沒把我給你的蘋果吃掉！？吃完你的HP就會上升了。請將血量回覆到100%再來找我吧。")
        return
    end
    self->sayNext("是不是很簡單？你可以在右側的欄位設定#b熱鍵#k。聽不懂對吧？每隔一段時間，血量就會恢復了。雖然很花時間，但好好運用的話可以幫助不少的。")
    self->sayNext("好了！我已經教你很多了，該給你禮物了。這是一些未來能幫助你在楓之谷世界存活的一些物品，請在緊急的時候使用它們。");
    self->sayNext("嗯，我能教你的就只有這些了，雖然很遺憾，但是我們真的該說再見了。好好照顧自己吧。\r\n\r\n#fUI/UIWindow.img/QuestIcon/4/0#\r\n#v2010000# 3 #t2010000#\r\n#v2010009# 3 #t2010009#\r\n\r\n#fUI/UIWindow.img/QuestIcon/8/0# 10 exp");

    target->incEXP(10);
    inventory->exchange(0, 2010000, 3);
    inventory->exchange(0, 2010009, 3);
    qr->setState(questID, 2)
end