require "./DataSrv/Script/sysDef"

if(self->askYesNo("現在就要離開這裡嗎？") == 1)
    target->transferField(103000000, "")