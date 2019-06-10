require "./DataSrv/Script/sysDef"
require "./DataSrv/Script/Npc/job_3rd"

nJob = target->getJob();
if(nJob == 0) --1st job advancement
{
    if(target->getLevel() >= 10)
    {
        if(self->askYesNo("看來你有充分的天賦與資質，你想成為#r海盜#k嗎？") == 1)
        {
            self->sayNext("這是最後的機會，一旦決定了就永遠無法改變了。");
            if(self->askYesNo("你確定真的想成為#r海盜#k？") == 1)
            {
                target->setJob(500);
                target->incSP(1 + (target->getLevel() - 10) * 3);
                inventory->exchange(0, 1492000, 1);
                inventory->exchange(0, 1482000, 1);
                inventory->exchange(0, 2330000, 1);
                self->say("希望你在未來的海盜道路上能夠順利，祝福你！");
            }
            else
                self->say("等你想清楚做好決定後再與我談話吧！");
        }
        else
            self->say("明智的選擇...");
    }
    else
        self->say("我可以看出你的潛能，但目前你還需多加努力，有朝一日才有可能成為#r海盜#k。");
}
else if(math.floor(nJob / 10) == 50) --2nd job advancement
{
    if(target->getLevel() < 30)
        self->say("成為海盜之後過的還好嗎？什麼？你想要成為更強大的海盜？嗯...目前看起來你尚未有資格成為#r打手#k或者#r槍手#k。");
    else
    {
        if(self->askMenu("有想說的話嗎？\r\n#L1##b想知道海盜是什麼#l") == 1)
        {
            if(qr->getState(2191) == 2 or qr->getState(2192) == 2)
            {
                nItemID = 4031856;
                jobIndex = 1;
                if(qr->getState(2192) == 2)
                {
                    jobIndex = 2;
                    nItemID = 4031857;
                }

                jobName = { "打手", "槍手" };
                jobCode = { 510, 520 };
                self->sayNext("果然不出我所料，你果真順利通過試驗了！我發現你在試煉場所的身手極為不凡呢～真厲害！就如當初的約定，我就讓你轉職為#b" ..jobName[jobIndex] .. "#k吧！");
                
                target->setJob(jobCode[jobIndex]);
                target->incSP(1);
                target->incMHP(random(300, 350));
                target->incMMP(random(150, 200));
                inventory->incSlotCount(4, 4);
                inventory->exchange(0, nItemID, -15);
                
                self->sayNext("那從現在開始你就是#b" ..jobName[jobIndex] .. "了#k。永遠不要停止履行海盜使命的腳步...總有一天，我還會讓你變得更加強大的！");
                self->sayNext("剛剛我提升了你的最大HP與MP，除此之外我有給你一些#b技能點數(SP)#k，打開#b技能視窗#k後確認看看吧！你可以把點數配給到二轉技能上，但無法一開始就全部設完，因為有些技能需要當前置技能的等級提升到某個階段後才能學習。");
                self->sayNext(jobName[jobIndex] .. "必須要變得很強！但若將自身的力量發洩在弱者身上，這並不是正確的方法。將自己所擁有的力量用在正確的事情上，這是比變得更強更為重要的課題。好了！相信你不斷自我修練，過不久就會再與我相見的，我期待那天的到來。");
            }
            else
            {
                nItemID = 0;
                nSkillID = 0;
                nFieldID = 0;
                if(qr->getState(2191) == 1) 
                {
                    --9001006
                    nItemID = 4031856;
                    nSkillID = 5001001;
                    nFieldID = 108000502 + random(0, 1);
                }
                else if(qr->getState(2192) == 1)
                {
                    --9001005
                    nItemID = 4031857;
                    nSkillID = 5001003;
                    nFieldID = 108000500 + random(0, 1);
                }
    
                if(nItemID != 0)
                {
                    self->sayNext("來！讓我來將擬送往試煉的場所吧！讓我再提醒你一次，請你打倒出現在那裡的海盜章魚後，蒐集#b#t" .. nItemID .. "#15個#k。出現在試煉場所的海盜章魚是經過特殊訓練，身手非常敏捷，因此要格外小心應付。");
                    self->sayNext("為了這次的訓練，出現在那裡的章魚們只能用#s" .. nSkillID .. "#來攻擊！另外當你進入試煉場所後，會將你所擁有的#t" .. nItemID .. "#全數回收，也就是從零重新開始！");
                    nCount = inventory->itemCount(nItemID);
                    inventory->exchange(0, nItemID, -nCount);
                    target->transferField(nFieldID, "");
                }
                else
                    self->say("請先選擇其中一個海盜考驗！");
            }
        }
    }
}
else if((nJob == 510 or nJob == 520) and target->getLevel() >= 70) --3rd job advancement
    job3rdMissionStart(2020013, "火獨眼獸洞穴");