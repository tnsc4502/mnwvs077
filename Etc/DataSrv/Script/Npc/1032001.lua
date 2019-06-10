
require "./DataSrv/Script/sysDef"

function magician2ndMissionDone()
{
    jobName = { "火毒巫師", "冰雷巫師", "僧侶" };
    jobCode = { 210, 220, 230 };
    self->sayNext("你可以平安地回到這裡...非常好。現在我知道你能夠應付那些簡單的任務了，接下來我要把你訓練得更加強大。但是在那之前，你有三條不同的道路選擇，我會給你一些時間思考，如果你有任何問題，也可以提出。");
    opt = self->askMenu("如果你已經決定好了，那麼請點選最底端的[我要決定職業]。\r\n#b#L0#請告訴我有關火毒巫師的事情。#l\r\n#L1#請告訴我有關冰雷巫師的事情。#l\r\n#L2#請告訴我有關僧侶的事情。#l\r\n#L3#我要決定職業。#l");
    if(opt == 3)
    {
        opt = self->askMenu("現在，你腦中有想法了嗎？那麼請選擇你要轉殖的二次職業：\r\n#b#L1#" .. jobName[1] .. "#l\r\n#L2#" .. jobName[2] .. "#l\r\n#L3#" .. jobName[3] .."#l");
        if(opt >= 1 and opt <= 3)
        {
            if(self->askYesNo("所以你的第二次轉職決定成為#b" .. jobName[opt] .. "#k嗎？在決定之後你再也無法反悔重新選擇...你真的確定嗎？") == 1 and (inventory->exchange(0, 4031012, -1) == 0))
            {
                target->setJob(jobCode[opt]);
                target->incSP(1);
                target->incMHP(random(300, 350));
                target->incMMP(random(150, 200));
                inventory->incSlotCount(4, 4);
                qr->setState(7500, 1, "");
                self->say("那從現在開始你就是#b" ..jobName[opt] .. "了#k。永遠不要停止履行法師使命的腳步...總有一天，我還會讓你變得更加強大的！");
            }
            else
                self->say("等你仔細思考完畢以後再跟我對談...");
        }
    }
}

nJob = target->getJob();

if(nJob == 0) --1st job advancement
{
    if(target->getLevel() >= 8)
    {
        if(self->askYesNo("看來你有充分的天賦與資質，你想成為#r法師#k嗎？") == 1)
        {
            self->sayNext("這是最後的機會，一旦決定了就永遠無法改變了。");
            if(self->askYesNo("你確定真的想成為#r法師#k？") == 1)
            {
                target->setJob(200);
                target->incSP(1 + (target->getLevel() - 8) * 3);
                inventory->exchange(0, 1372005, 1);
                self->say("希望你在未來的法師道路上能夠順利，祝福你！");
            }
            else
                self->say("等你想清楚做好決定後再與我談話吧！");
        }
        else
            self->say("明智的選擇...");
    }
    else
        self->say("我可以看出你的潛能，但目前你還需多加努力，有朝一日才有可能成為#r法師#k。");
}
else if(math.floor(nJob / 10) == 20) --2nd job advancement
{
    if(target->getLevel() < 30)
        self->say("成為法師之後過的還好嗎？什麼？你想要成為更強大的法師？嗯...目前看起來你尚未有資格成為#r火/毒巫師#k、#r冰/雷魔法師#k或者#r僧侶#k。")
    else
    {
        qrVal = qr->get(7500);
        if(qrVal == "s")
        {
            if(inventory->itemCount(4031012) == 1)
                magician2ndMissionDone();
            else if(inventory->itemCount(4031009) == 0)
            {
                self->say("看來你把信封給弄丟了對吧！我可以再給你一次機會，請把信封給保管好！");
                if(inventory->exchange(0, 4031009, 1) != 0) 
                    self->say("請確保背包的其他欄有足夠的欄位。");
            }
            else
                self->say("你還沒去找#b法師轉職教官#k嗎？請把握時間把信封交給他並完成他所交代的任務再回來找我。");
        }
        else if(self->askYesNo("嗯...看來這段時間你進步的非常的快，看起來與之前相當不同。當時你看起來非常的弱小，現在我可以向你保證，你已經有法師該有的樣子...那你覺得呢？你想不想要獲得更強大的力量？通過一些簡單的考驗我就能幫你...你準備好了嗎？") == 1)
        {
            self->sayNext("非常好...看起來你非常有勇氣，但我需要親自測試看看是不是真的。這個考驗並不會太難，你應該能夠通過的。首先，先把這封信帶在身上，記住不要弄不見了。");
            if(inventory->exchange(0, 4031009, 1) != 0) 
            {
                qr->setState(7500, 1, "s");
                self->sayNext("請把這封信交給位於#r魔法森林北部#k的#b法師轉職教官#k。他在那邊進行轉職的指導工作...把這封信交給他然後他會把你帶到測試地點。詳細的說明將由他告訴你。");
            }            
            else
                self->say("請確保背包的其他欄有足夠的欄位。");
        }
        else
            self->say("看來你還有事情沒有完成，等你完成並且想好後再來跟我說吧...");
    }
}
else if((nJob == 210 or nJob == 220 or nJob == 230) and target->getLevel() >= 70) --3rd job advancement
{
    qrVal = qr->get(7500);
    if(qrVal == "s")
    {
        qr->setState(7500, 1, "p1");
        self->sayNext("我一直在等著你。幾天前我從#b#p2020009##k那邊聽到你的事情。嗯...我必須先測驗你的能耐。在魔法森林某處有個秘密通道。除了你以外沒有人能夠進入。如果你找到那個通道，你會看到我的另一個分身，打敗他並且帶回#b#t4031059##k給我。");   
        self->sayNext("我的分身非常強悍。他會運用多種特殊技能，你必須1對1打敗他。而且正常人並無法在裡面待太長的時間，所以你必須在有限時間內擊敗他。總之...祝你好運，我很期待你把#b#t4031059##k帶回來給我。");
    }
    else if(qrVal == "p1")
    {
        nCount = inventory->itemCount(4031059);
        if(nCount > 0)
        {
            self->sayNext("哇，你打敗了我的分身並且帶來了#b#t4031059##k給我。這證實了你的實力已經具備可以進行第三次轉職了，如我所承諾，我會給你#b#t4031057##k，把這個帶回給#b#p2020009##k，接著你就可以進行第二階段的挑戰，祝你好運～");
            if(inventory->exchange( 0, 4031059, -1, 4031057, 1) == 0)
                qr->setState(7500, 1, "p2");
            else
                self->say("你確定真的有帶來#b#t4031059##k嗎？或者你的背包沒有足夠的空間可以存放#b#t4031057##k。");
        }
        else
            self->say("你還沒找到秘密通道並且擊敗我的分身嗎？請趕緊找到通道並且擊敗他並帶回#b#t4031059##k給我。");
    }
    else if(qrVal == "p2")
    {
        if(inventory->itemCount(4031057) == 0)
        {
            self->say("你把#b#t4031057##k給弄丟了嗎？這是最後一次機會了，請小心保管。");
            if(inventory->exchange(0, 4031057, 1) != 0)
                self->say("請確保背包有足夠的欄位！");
        }
        else
            self->say("還沒把#b#t4031057##k交給#b#p2020009##k並進行第二個測驗嗎？");
        
    }
}