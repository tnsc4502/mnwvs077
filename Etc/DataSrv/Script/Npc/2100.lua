
require "./DataSrv/Script/sysDef"

field = self->getField();
if(field and field->getID() == 0) 
{
    if(self->askYesNo("歡迎來到楓之谷世界！這裡的訓練所是為了要幫助初心者～我會建議您利用訓練所來熟悉遊戲，當然您也可以直接略過並開始冒險，是否要進入訓練場呢？") == 1)
    {
        self->say("我將送您進入，但是在那之前還是要提醒您，請務必遵循指導員的指示，祝您好運～");
        target->transferField(1, "");
    }
    else
    {
        if(self->askYesNo("您是否真的要直接開始旅程呢？") == 1) 
        {
            self->say("看來您已經下定決心，那我將直接將您送離訓練所，祝您好運～");
            target->transferField(40000, "");
        }
        else
            self->say("看來您好沒有準備好，等您準備好後隨時可以跟我說！");
    }
}
else
{
    self->sayNext("這裡將會開始訓練課程，同時您也有機會可以體驗各式的職業。");
    self->sayNext("當您努力到達了一定的等級後，就可以選擇職業，在弓箭手村可以轉職為弓箭手，魔法森林可以轉職為法師，勇士之村可以轉職為劍士，墮落城市則可以轉職為盜賊！");
}