
require "./DataSrv/Script/sysDef"

--Just followed how offical server did.
field = qr->get(7600);
transferID = 100000100;
if(field != "")
    transferID = tonumber(field);

target->transferField( transferID, "market00" );