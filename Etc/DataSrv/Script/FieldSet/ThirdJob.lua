require "./DataSrv/Script/sysDef"

fieldSet = FieldSet.get()

function onTimeout(forceClose) 
{
    fieldSet = FieldSet.get();
    fieldSet->transferAll(tonumber(fieldSet->getVar("returnField")), "");
}