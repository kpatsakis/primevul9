void ZRtp::resetSASVerified() {

    zidRec->resetSasVerified();
    getZidCacheInstance()->saveRecord(zidRec);
}