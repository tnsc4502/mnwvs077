require "./DataSrv/Script/sysDef"

local s = 0

function onTimeout()
    --s = s + 1
    fieldSet = FieldSet.get("Party2")
    fieldSet->transferAll(922010000, "")
    self->debug("[Party2]On Time out s = " )
end

function onUserEnter(fieldID, userID)
    --s = s + 1
    --fieldSet->setVar("a", "0")
    fieldSet = FieldSet.get("Party2")
    self->debug("[Party2]On User Enter, FieldID = " .. fieldID .. " User ID = " .. fieldSet->getVar("LeaveFieldID") .. " s = ")
end

self->debug("[Party2]On Initialize ")