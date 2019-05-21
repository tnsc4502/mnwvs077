require "./DataSrv/Script/sysDef"

if(self->askYesNo("現在就要離開這裡嗎？") == 1) then
    target->transferField(103000000, "")
end