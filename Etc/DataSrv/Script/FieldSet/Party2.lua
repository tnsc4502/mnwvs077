require "./DataSrv/Script/sysDef"

function onTimeout(forceClose) {
    fieldSet = FieldSet.get();
    if(forceClose == 1 or fieldSet->getVar("stage") != "clear_end")
        fieldSet->transferAll(922010000, "");
    else
        fieldSet->transferAll(922011100, "");
}

function onUserEnter(fieldID, userID) {
    fieldSet = FieldSet.get();
    --Send all members to bonus map
    if(fieldID == 922011000 and fieldSet->getVar("stage") == "clear")
    {
        fieldSet->resetTimeLimit(60, 1);
        fieldSet->setVar("stage", "clear_end");
    }
}
