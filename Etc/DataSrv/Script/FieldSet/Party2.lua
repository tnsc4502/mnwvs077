require "./DataSrv/Script/sysDef"

function onTimeout(forceClose) {
    fieldSet = FieldSet.get()
    fieldSet->transferAll(922010000, "")
    self->debug("[Party2]On Time out s = " )
}

function onUserEnter(fieldID, userID) {
    fieldSet = FieldSet.get()
    self->debug("[Party2]On User Enter, FieldID = " .. fieldID .. " User ID = " .. fieldSet->getVar("LeaveFieldID") .. " s = ")
}

self->debug("[Party2]On Initialize ")