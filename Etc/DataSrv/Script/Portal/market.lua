require "./DataSrv/Script/sysDef"

function s_market00() {
    --Just followed how offical server did.
    field = qr->get(7600);
    transferID = 100000100;
    if(field != "")
        transferID = tonumber(field);

    target->transferField( transferID, "market00" );
}

function s_market01() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market02() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market03() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market04() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market05() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market06() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market07() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market08() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market09() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market10() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market11() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market12() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market13() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}

function s_market14() {
    qr->setState(7600, 1, self->getField()->getID());
    target->transferField( 910000000, "out00" );
}
