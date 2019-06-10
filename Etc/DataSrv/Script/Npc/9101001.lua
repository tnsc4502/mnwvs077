
require "./DataSrv/Script/sysDef"

self->sayNext("非常好！您完成了所有的訓練。似乎您已經可以開始冒險的旅程！祝您好運，我這就帶您離開。");
self->sayNext("順帶一提，一旦離開這裡，您將會到達充滿怪物的村莊。再見了！");

target->incEXP(3);
target->transferField(40000, "");