
require "./DataSrv/Script/sysDef"

function changeJob4(jobCode, skillLearnItem)
{
    nPSP = (target->getLevel() - 120) * 3;
    if(target->getSP(0) > nPSP)
        self->say("зAж№еG┴┘ж││╤╛lк║#bз▐пр┬I╝╞#kй|е╝и╧е╬з╣▓жбI╜╨зт│╤╛lк║з▐пр┬I╝╞и╧е╬з╣▓жлсжAи╙╗Pз┌╣я╕▄бC");
    else
    {
        if(inventory->exchange(0, skillLearnItem, 1) != 0)
        {
            self->say("╜╨╜TлOнIе]к║о°п╙─ц┴┘ж│им░ўк║к┼╢ббC");
            return;
        }
        target->setJob(jobCode + 1);
        target->incAP(5);
        target->incSP(3);

        nJobType = math.floor(jobCode / 100);
        if(nJobType == 2)
        {
            if(jobCode == 111)
                self->sayNext("ое│▀зAжим░дF╝Cдhе@м╔к║#b▓╫╖ен^╢п#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 121)
                self->sayNext("ое│▀зAжим░дF╝Cдhе@м╔к║#bе·дз╕t├Mдh#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 131)
                self->sayNext("ое│▀зAжим░дF╝Cдhе@м╔к║#b╢┬├Mдh#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            self->sayNext("│oи├лD╝Cдhдз╕Їк║▓╫┬IбAзAк║╝Cдhдз╕Ї▒Nи·иMйєзAжpжє┴ы┴хк║ж█дvбC");
        }
        else if(nJobType == 2)
        {
            if(jobCode == 211)
                self->sayNext("ое│▀зAжим░дF┼]кkе@м╔к║#bдj┼]╛╔дhб]дїбBмrб^#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 221)
                self->sayNext("ое│▀зAжим░дF┼]кkе@м╔к║#bдj┼]╛╔дhб]жBбB╣pб^#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 231)
                self->sayNext("ое│▀зAжим░дF┼]кkе@м╔к║#bеD▒╨#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            self->sayNext("│oи├лDкkоvдз╕Їк║▓╫┬IбAзAк║кkоvдз╕Ї▒Nи·иMйєзAжpжє┴ы┴хк║ж█дvбC");
        }
        else if(nJobType == 3)
        {
            if(jobCode == 311)
                self->sayNext("ое│▀зAжим░дFд}╜bдте@м╔к║#b╜bпл#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 321)
                self->sayNext("ое│▀зAжим░дFд}╜bдте@м╔к║#bплоgдт#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            self->sayNext("│oи├лDд}╜bдтдз╕Їк║▓╫┬IбAзAк║д}╜bдтдз╕Ї▒Nи·иMйєзAжpжє┴ы┴хк║ж█дvбC");
        }
        else if(nJobType == 4)
        {
            if(jobCode == 411)
                self->sayNext("ое│▀зAжим░дF╡s╕ще@м╔к║#bй]и╧О═#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 421)
                self->sayNext("ое│▀зAжим░дF╡s╕ще@м╔к║#b╖t╝vпл░╜#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            self->sayNext("│oи├лD╡s╕щдз╕Їк║▓╫┬IбAзAк║╡s╕щдз╕Ї▒Nи·иMйєзAжpжє┴ы┴хк║ж█дvбC");
        }
        else if(nJobType == 5)
        {
            if(jobCode == 511)
                self->sayNext("ое│▀зAжим░дFо№╡sе@м╔к║#bо▒┼Q#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            else if(jobCode == 521)
                self->sayNext("ое│▀зAжим░дFо№╡sе@м╔к║#b║jпл#kбC╢}йl╡o┤з┼]дOк║╡Lнн╝чпрзaбI");
            self->sayNext("│oи├лDо№╡sдз╕Їк║▓╫┬IбAзAк║о№╡sдз╕Ї▒Nи·иMйєзAжpжє┴ы┴хк║ж█дvбC");
        }
    }
}