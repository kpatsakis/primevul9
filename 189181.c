void ZRtp::SASVerified() {
    if (paranoidMode)
        return;

    zidRec->setSasVerified();
    getZidCacheInstance()->saveRecord(zidRec);
}