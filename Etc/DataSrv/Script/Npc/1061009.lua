
require "./DataSrv/Script/sysDef"

nJob = target->getJob();
nJobType = math.floor(nJob / 100);
nJobAdv = (math.floor(nJob / 10) % 10);
nJobSub = nJob % 10;

if(nJobType != 0 and nJobSub == 0 and nJobAdv != 0 and qr->get(7500) == "p1")
{
    fieldSet = FieldSet.get("ThirdJob" .. nJobType);
    if(fieldSet)
    {

        ret = fieldSet->enter(userID);
        if (ret == 5) 
        {
            self->say("已經有其他冒險家進行中，請稍待片刻。");
            return;
        }
        fieldSet->setVar("returnField", self->getField()->getID());
    }
}
else
    self->say("看來無法進入...");