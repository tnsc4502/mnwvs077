math.randomseed(os.time())

target = User.get(userID)
if(target) then
    inventory = target->inventory()
    qr = target->questRecord()
end

--Npc.get() will only return a unique pointer in a session.
self = Npc.get()

--Generate a random number between [min, max)
function random(min, max)
   return System.random(min, max)
end

--Generate a random sequence that contains 'count' different numbers which start with 'numStart'
function randomSeq(numStart, count)
    initSeq = {}
    for i = 1, count do
        table.insert(initSeq, i)
    end
    retSeq = {}
    tryCount = count
    for i = 1, tryCount do
        idx = random(1, count + 1)
        table.insert(retSeq, initSeq[idx])
        table.remove(initSeq, idx)
        count = count - 1
    end
    return retSeq
end