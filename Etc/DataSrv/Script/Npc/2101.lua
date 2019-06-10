
require "./DataSrv/Script/sysDef"

if(self->askYesNo("您已經完成訓練想要離開這裡了嗎？") == 1) 
{
    self->say("我將送你離開，再會了。");
    target->transferField( 3, "" );
}
else
    self->say("您還沒完成訓練計畫嗎？如果你想離開這裡就直接告訴我吧。");