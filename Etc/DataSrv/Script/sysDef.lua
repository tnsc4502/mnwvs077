math.randomseed(os.time());

target = User.get(userID);
if(target) {
    inventory = target->inventory();
    qr = target->questRecord();
}

--Npc.get() will only return a unique pointer in a session.
self = Npc.get();

--Generate a random number between [min, max]
function random(min, max) {
   return System.random(min, max + 1);
}

--Generate a random sequence that contains 'count' different numbers which start with 'numStart'
function randomSeq(numStart, count) {
    initSeq = {};
    for i = 1, count
        table.insert(initSeq, i);

    retSeq = {};
    tryCount = count;
    for i = 1, tryCount {
        idx = random(1, count);
        table.insert(retSeq, initSeq[idx]);
        table.remove(initSeq, idx);
        count = count - 1;
    }
    return retSeq;
}

function currentTime() {
    return System.currentTime();
}

function compareTime(t1, t2) {
    return System.compareTime(t1, t2);
}

function substring(str, start, count) {
    return string.sub(str, start + 1, start + count);
}

function dayOfWeek() {
    return System.dayOfWeek();
}

--For generating a character sequence which contains only 0 and 1 for PQs.
function make_area_answer(numCount, answerCount) {
    rndSeq = randomSeq(1, numCount);
    ret = "";
    for k, v in pairs(rndSeq) {
        if(v > answerCount)
            ret = ret .. '0';
        else
            ret = ret .. '1';
    }
    return ret;
}