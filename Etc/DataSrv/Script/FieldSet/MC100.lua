require "./DataSrv/Script/sysDef"

stage = 0;
lastUpdate = 0;
fieldSet = FieldSet.get()

--stage : 
-- 0 = On Waiting requests
-- 1 = On Waiting to start
-- 2 = On MC Battling
-- 3 = On Game has ended
-- 4 = On Taking rewards

function onTimeout(forceClose) 
{
    if(stage == 4 or forceClose)
        fieldSet->transferAll(980000000, "");
    
    if(stage == 2)
    {
        stage = 3;
        fieldSet->resetTimeLimit(10, 1);
    } 
    else if(stage == 3) 
    {
        stage = 4;
        fieldSet->resetTimeLimit(30, 1);
        fieldSet->transferAll(980000303, "");
    }

    self->debug("On Time out s = " ..stage );
}

function enterMCarnival()
{
    stage = 2
    --self->debug("Try Enter MC. MCParty1 = " .. fieldSet->getVar("MCParty1") .. " MCParty2 = " .. fieldSet->getVar("MCParty2"));
    fieldSet->resetTimeLimit(600, 1);
    fieldSet->setVar("timeLimit", "600");

    fieldSet->transferParty(tonumber(fieldSet->getVar("MCParty1")), 980000301, "red00")
    fieldSet->transferParty(tonumber(fieldSet->getVar("MCParty2")), 980000301, "blue00")
}

function onUserEnter(fieldID, userID) 
{
    if(fieldSet->getVar("MCLeader1") == "") 
    {
        target = User.get(userID);
        fieldSet->setVar("MCLeader1", userID);
        fieldSet->setVar("MCParty1", target->getPartyID());
    }
    self->debug("On User Enter, FieldID = " .. fieldID .. " User ID = " .. userID)
}

function onUpdate(tCur)
{
    if(stage == 1 and (tCur - lastUpdate > 3 * 1000))
        enterMCarnival();
}

function onJoin(userID)
{
    target = User.get(userID);
    fieldSet->setVar("MCLeader2", userID)
    fieldSet->setVar("MCParty2", target->getPartyID());
    stage = 1;
    lastUpdate = System.getTime();
}

function onInit() 
{
    stage = 0;
    lastUpdate = 0;
    self->debug("On Initializing MC100 FielSet.")
}