
require "./DataSrv/Script/sysDef"
require "./DataSrv/Script/Npc/job_3rd"

nJob = target->getJob();
if((nJob == 310 or nJob == 320) and target->getLevel() >= 70)
{
    qrVal = qr->get(7500);
    if(qrVal == "")
    {
        if(self->askYesNo("歡迎。我的名字叫做#b#p" .. templateID .. "##k，是弓箭手界的最高負責人，時時刻刻將我的所學以及知識教導給世界各地的弓箭手。你看起來已經準備好接受第三次轉職的挑戰，但我覺得你不可太過自信，我看過太多弓箭手來來去去，在挑戰的過程中讓我大為失望。總而言之，你準備好接受第三次轉職的挑戰了嗎？") == 1)
        {
            qr->setState(7500, 1, "s");
            self->sayNext("非常好。在這個測驗中有兩個考察的重點：力量與智慧。關於體能方面的挑戰我會慢慢解釋給你聽。記得弓箭手村的#b赫麗娜#k嗎？去找他，他會給你任務指示，將任務完成並且把#b#t4031057##k帶回來給我。");
            self->sayNext("關於心智方面的挑戰會在你完成體力測驗後才會開始。#b#t4031057##k是你通過挑戰的證明。#b赫麗娜#k會負責監督這個挑戰。這不會像你所想的那般容易，但我對你期望很高，祝你好運。");
        }
        else
            self->say("是嗎，看起來你還沒準備好。有朝一日等你準備好時，隨時可以來找我接受挑戰。");
    }
    else if(qrVal == "s" or qrVal == "p1") { self->say("你還沒帶來#b#t4031057##k嗎？這是證明你實力的必要考驗，沒有通過考驗的人是無法獲得更強大的力量的...");}
    else if(qrVal == "p2")
        job3rdMissionFinalStage1(target->getJob(), "赫麗娜");
    else if(qrVal == "end1" or qrVal == "end2")
        job3rdMissionFinalStage2(target->getJob(), "弓箭手");
}