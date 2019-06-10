require "./DataSrv/Script/sysDef"

count  = 0; 
fieldSet = FieldSet.get()

function onTimeout(forceClose) {
    --s = s + 1
    fieldSet->transferAll(103000890, "")
    self->debug("On Time out s = " )
}

function onUserEnter(fieldID, userID) {
    --s = s + 1
    --fieldSet->setVar("a", "0")
    fieldSet = FieldSet.get()
    count = count + 1;
    self->debug("On User Enter, FieldID = " .. fieldID .. " User ID = " .. fieldSet->getVar("LeaveFieldID") .. " s = " .. count)
}

function initialize() {
    count = 0;
}