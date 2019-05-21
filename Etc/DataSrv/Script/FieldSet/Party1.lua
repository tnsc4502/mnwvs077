require "./DataSrv/Script/sysDef"

local s = 0

function onTimeout()
    --s = s + 1
    fieldSet = FieldSet.get("Party1")
    fieldSet->transferAll(103000890, "")
    self->debug("On Time out s = " )
end

function onUserEnter(fieldID, userID)
    --s = s + 1
    --fieldSet->setVar("a", "0")
    fieldSet = FieldSet.get("Party1")
    self->debug("On User Enter, FieldID = " .. fieldID .. " User ID = " .. fieldSet->getVar("LeaveFieldID") .. " s = ")
end

self->debug("On Initialize ")