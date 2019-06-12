
require "./DataSrv/Script/sysDef"

function s_guild_mark() {
    isGuildMaster = target->isGuildMaster();
    if(isGuildMaster == 1)
    {
        nRet = self->askMenu("您好，請問有什麼我可以幫忙的嗎？#b\r\n#L0#設定公會勳章#l\r\n#L1#移除公會勳章#l");
        if(nRet == 0)
        {
            if(self->askYesNo("設定公會勳章需要#b500萬楓幣#k，你確定要繼續嗎？") == 1)
            {       
                if(target->isGuildMarkExist() == 1)
                    self->say("你的公會已經設定了勳章，請先移除後再重新設定。");
                else
                    target->setGuildMark();
            }
            else
                self->say("等你想好以後再來與我對談吧。");
        }
        else if(nRet == 1)
        {
            if(self->askYesNo("移除公會勳章需要#b100萬楓幣#k，你確定要繼續嗎？") == 1)
            {       
                if(target->isGuildMarkExist() == 0)
                    self->say("你的公會並沒有設定勳章，無法進行移除。");
                else
                    target->removeGuildMark();
            }
            else
                self->say("等你想好以後再來與我對談吧。");
        }
    }
    else
        self->say("有公會會長有足夠的權限可以管理公會勳章。");
}

function s_guild_proc() {
    isGuildMaster = target->isGuildMaster();
    isGuildMember = target->isGuildMember();
    nCountMax = target->getGuildCountMax();

    if(isGuildMaster == 1)
        nRet = self->askMenu("我能幫您什麼忙？\r\n#b#L1#解散公會#l\r\n#L2#擴充公會人數(上限：100)#l#k");
    else
        nRet = self->askMenu("我能幫您什麼忙？\r\n#b#L0#建立公會#l\r\n#L3#說明有關公會的事情#l#k");
        
    if(nRet == 0)
    {
        if(isGuildMember == 1)
        {
            self->say("您已經加入公會。");
            return;
        }
        if(self->askYesNo("登記公會的費用是#b150萬楓幣#k，您確定要建立一個新公會嗎？") == 1)
            target->createNewGuild();
        else
            self->say("等你仔細思考後再來找我吧。");
    }
    else if(nRet == 1 and isGuildMember)
    {
        if(self->askYesNo("公會一旦解散後就無法復原，也不會退還所有扣除過的費用且必須追加#b20萬楓幣#k手續費，公會所獲得的#b公會點數(GP)#k也會全數消失。你確定要繼續嗎？") == 1)
            target->removeGuild();
        else
            self->say("等你仔細思考後再來找我吧。");
    }
    else if(nRet == 2 and isGuildMaster)
    {
        nCountMax = target->getGuildCountMax(); 
        if(nCountMax > 95) self->say("您的公會人數已達上限，無法再進行擴充。");
        else
        {
            self->sayNext("覺得公會人數上限不夠了嗎？我可以幫你擴充公會的最大上限人數，當然，需要一些額外的手續費。");
            nRequiredMeso = 500;
            if ( nCountMax <= 10 ) nRequiredMeso = 50; 
            else if ( nCountMax <= 15 ) nRequiredMeso = 150; 
            else if ( nCountMax <= 20 ) nRequiredMeso = 250; 
            else if ( nCountMax <= 25 ) nRequiredMeso = 350; 
            else if ( nCountMax <= 30 ) nRequiredMeso = 450; 
            else if ( nCountMax <= 35 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 40 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 45 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 50 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 55 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 60 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 65 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 70 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 75 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 80 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 85 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 90 ) nRequiredMeso = 500; 
            else if ( nCountMax <= 95 ) nRequiredMeso = 500; 
            if(self->askYesNo("想要把公會人數上限擴充至" .. (nCountMax + 5) .. "人，必須要支付手續費:#b" .. nRequiredMeso .. "萬楓幣#k，您確定要繼續嗎？") == 1)
                target->incGuildCountMax(5, nRequiredMeso * 10000);
            else
                self->say("等你想好以後再來找我談話吧。");
        }
    }
}