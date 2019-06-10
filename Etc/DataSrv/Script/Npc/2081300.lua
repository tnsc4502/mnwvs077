require "./DataSrv/Script/sysDef"
require "./DataSrv/Script/Npc/job_4th"

nJob = target->getJob();
if((nJob != 311 and nJob != 321) or target->getLevel() < 120)
{
    self->say("有什麼事情嗎？");
    return;
}

if(qr->getState(6924) == 2)
{
    ret = self->askMenu("看來你已經準備好成為真正的弓箭手了。\r\n你想要進行第四次轉職嗎？#b\r\n#L0#我想進行第四次轉職#l\r\n#L1#讓我在想想#l");
    if(ret == 1)
        self->sayNext("其實沒有什麼好猶豫的。等你決定好之後再來跟我交談。如果你真的準備好了，我會讓你進行第四次轉職的。");
    else
        changeJob4(nJob, 2280003);
}
else
    self->say("你還沒為四轉做好準備，等你準備好了，再來找我吧。");