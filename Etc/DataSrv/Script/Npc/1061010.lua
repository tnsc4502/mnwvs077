
require "./DataSrv/Script/sysDef"

fieldID = self->field()->getID();
returnField = 0;
if(fieldID == 108010301)
    returnField = 102000000;
else if(fieldID == 108010201)
    returnField = 101000000;
else if(fieldID == 108010101)
    returnField = 100000000;
else if(fieldID == 108010401)
    returnField = 103000000;
else
    returnField = 120000101;

if(self->askYesNo("已經完成任務想要離開了嗎？我可以送你回到#b#m" .. returnField .. "##k。") == 1)
    target->transferField(returnField, "");