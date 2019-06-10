
require "./DataSrv/Script/sysDef"

if(self->askYesNo("已經完成任務想要離開了嗎？我可以送你回到魔法森林。") == 1)
    target->transferField(101000000, "");