void ZRtp::acceptEnrollment(bool accepted) {
    if (!accepted) {
        zidRec->resetMITMKeyAvailable();
        callback->zrtpInformEnrollment(EnrollmentCanceled);
        getZidCacheInstance()->saveRecord(zidRec);
        return;
    }
    if (pbxSecretTmp != NULL) {
        zidRec->setMiTMData(pbxSecretTmp);
        getZidCacheInstance()->saveRecord(zidRec);
        callback->zrtpInformEnrollment(EnrollmentOk);
    }
    else {
        callback->zrtpInformEnrollment(EnrollmentFailed);
    }
    return;
}