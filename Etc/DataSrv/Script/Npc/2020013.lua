
require "./DataSrv/Script/sysDef"
require "./DataSrv/Script/Npc/job_3rd"

nJob = target->getJob();
if((nJob == 510 or nJob == 520) and target->getLevel() >= 70)
{
    qrVal = qr->get(7500);
    if(qrVal == "")
    {
        if(self->askYesNo("嗯...你想要透過第三次轉職成為更加強大的海盜嗎？首先，我必須先說恭喜！因為很少人能夠像你這般堅持。我確實可以讓你變得更為強大，但是在那之前我必須先考驗你，並且確認你是否具有資格。很多人聲稱自己擁有力量與智慧，但卻很少人能夠通過考驗。你準備好進行測驗了嗎？") == 1)
        {
            self->sayNext("非常好。在這個測驗中有兩個考察的重點：力量與智慧。關於體能方面的挑戰我會慢慢解釋給你聽。記得鯨魚號的#b#p1090000##k嗎？去找他，他會給你任務指示，將任務完成並且把#b#t4031057##k帶回來給我。");
            self->sayNext("關於心智方面的挑戰會在你完成體力測驗後才會開始。#b#t4031057##k是你通過挑戰的證明。#b#p1090000##k會負責監督這個挑戰。這不會像你所想的那般容易，但我對你期望很高，祝你好運。");
            qr->setState(7500, 1, "s");
        }
        else
            self->say("是嗎，看起來你還沒準備好。有朝一日等你準備好時，隨時可以來找我接受挑戰。");
    }
    else if(qrVal == "s" or qrVal == "p1") { self->say("你還沒帶來#b#t4031057##k嗎？這是證明你實力的必要考驗，沒有通過考驗的人是無法獲得更強大的力量的...");}
    else if(qrVal == "p2")
        job3rdMissionFinalStage1(target->getJob(), "#p1090000#");
    else if(qrVal == "end1" or qrVal == "end2")
        job3rdMissionFinalStage2(target->getJob(), "海盜");
}