
require "./DataSrv/Script/sysDef"

--Final Stage Phase 1
function job3rdMissionFinalStage1(jobCode, npcName)
{
    if(inventory->exchange(0, 4031057, -1) == 0)
    {
        qr->setState(7500, 1, "end1");
        self->sayNext("非常好，你完成了第一階段的體能測驗，我就知道你辦的到！接下來你必須接受第二階段的心智測驗。");
        self->sayNext("這是第二階段測驗的說明。這個測驗將考驗你是否有足夠的智慧可以進行下一階段的轉職。在冰原雪域有個終年積雪的地方叫做#b雪原聖地#k，那個地方就連怪物都無法到達。在那邊有一個巨大的岩石叫做#b神聖的石頭#k，在那邊你必須貢獻出一些特殊物品，接著岩石就會考驗你的智慧。");
        self->sayNext("你必須誠實的回答每一個問題。如果你答對了所有的問題，聖石會給你#b#t4031058##k。將它帶回來給我，我就會幫助你進入下一個階段。");
    }
    else
        self->say("你確定身上有帶著#b#t4031057##k嗎？如果沒有，請回去與#b" .. npcName .. "#k談話，看看他是否還會給你機會...");
}

--Final Stage Phase 2
function job3rdMissionFinalStage2(jobCode, jobTypeName)
{
    if(qrVal == "end2" or inventory->itemCount(4031058) > 0)
    {
        self->sayNext("非常好，你完成了第二階段心智考驗。我必須說，我對你的所展現出智慧非常欽佩。在進入下一個階段之前，你必須先把項鍊交給我。");
        if(self->askYesNo("好的！現在，你即將成為更加強大的" .. jobTypeName .."，在那之前，請確保你的#b技能點數#k已經全部使用完畢。你必須把70等以前獲得的所有技能點數都使用完畢才可以進行第三次轉職。由於你已經在第二次轉職時做出了選擇，這次將不會有其他道路供你選擇。你現在就想要進行轉職嗎？") == 1)
        {
            if(qrVal != "end2" and inventory->exchange(0, 4031058, -1) != 0)
                self->say("你確定身上真的有帶著#b#t4031058##k嗎？");
            else
                changeJob3rd(target->getJob());
        }
        else
            self->say("等你認真思考完畢以後再來與我對話吧！");
    }
    else
        self->say("請去#b雪原聖地#k找到#b神聖之石#k並且回答所有問題後帶回#b#t4031058##k給我。");
}

function job3rdMissionStart(npcFrom, aroundName)
{
    qrVal = qr->get(7500);
    if(qrVal == "s")
    {
        qr->setState(7500, 1, "p1");
        self->sayNext("我一直在等著你。幾天前我從#b#p" .. npcFrom .. "##k那邊聽到你的事情。嗯...我必須先測驗你的能耐。在" .. aroundName .. "某處有個秘密通道。除了你以外沒有人能夠進入。如果你找到那個通道，你會看到我的另一個分身，打敗他並且帶回#b#t4031059##k給我。");   
        self->sayNext("我的分身非常強悍。他會運用多種特殊技能，你必須1對1打敗他。而且正常人並無法在裡面待太長的時間，所以你必須在有限時間內擊敗他。總之...祝你好運，我很期待你把#b#t4031059##k帶回來給我。");
    }
    else if(qrVal == "p1")
    {
        nCount = inventory->itemCount(4031059);
        if(nCount > 0)
        {
            self->sayNext("哇，你打敗了我的分身並且帶來了#b#t4031059##k給我。這證實了你的實力已經具備可以進行第三次轉職了，如我所承諾，我會給你#b#t4031057##k，把這個帶回給#b#p" .. npcFrom .. "##k，接著你就可以進行第二階段的挑戰，祝你好運～");
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
            self->say("還沒把#b#t4031057##k交給#b#p" .. npcFrom .. "##k並進行第二個測驗嗎？");
    }
}

function changeJob3rd(jobCode)
{
    qr->setState(7500, 1, "end2");
    nPSP = (target->getLevel() - 70) * 3;
    if(target->getSP(0) > nPSP)
        self->say("你似乎還有剩餘的#b技能點數#k尚未使用完畢！請把剩餘的技能點數使用完畢後再來與我對話。");
    else
    {
        qr->setState(7500, 2, "");
        target->setJob(jobCode + 1);
        target->incAP(5);
        target->incSP(1);
        self->say("(突然感受到力量變得更加強大了...)");
    }
}