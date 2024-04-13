void ZRtp::setRs2Valid() {

    if (zidRec != NULL) {
        zidRec->setRs2Valid();
        getZidCacheInstance()->saveRecord(zidRec);
    }
}