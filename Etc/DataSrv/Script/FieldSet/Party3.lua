require "./DataSrv/Script/sysDef"

function onTimeout(forceClose) 
{
    fieldSet = FieldSet.get();
    if(forceClose == 1 or fieldSet->getVar("bonus_stage") != "1")
        fieldSet->transferAll(920011200, "");
    else
        fieldSet->transferAll(920011300, "");
}

function onUserEnter(fieldID, userID)
{
    fieldSet = FieldSet.get();
    if(fieldID == 920011100 and fieldSet->getVar("bonus_stage") != "1") 
    {
        fieldSet->resetTimeLimit(60, 1);
        fieldSet->setVar("bonus_stage", "1");
    }
}
