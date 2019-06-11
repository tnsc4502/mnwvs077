
require "./DataSrv/Script/sysDef"

packet = Packet.new();
packet->encode2(9);
packet->send(target);