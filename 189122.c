void ZRtp:: computeSharedSecretSet(ZIDRecord *zidRec) {

    /*
     * Compute the Initiator's and Reponder's retained shared secret Ids.
     * Use negotiated HMAC.
     */
    uint8_t randBuf[RS_LENGTH];
    uint32_t macLen;

    detailInfo.secretsCached = 0;
    if (!zidRec->isRs1Valid()) {
        randomZRTP(randBuf, RS_LENGTH);
        hmacFunction(randBuf, RS_LENGTH, (unsigned char*)initiator, strlen(initiator), rs1IDi, &macLen);
        hmacFunction(randBuf, RS_LENGTH, (unsigned char*)responder, strlen(responder), rs1IDr, &macLen);
    }
    else {
        rs1Valid = true;
        hmacFunction((unsigned char*)zidRec->getRs1(), RS_LENGTH, (unsigned char*)initiator, strlen(initiator), rs1IDi, &macLen);
        hmacFunction((unsigned char*)zidRec->getRs1(), RS_LENGTH, (unsigned char*)responder, strlen(responder), rs1IDr, &macLen);
        detailInfo.secretsCached = Rs1;
    }

    if (!zidRec->isRs2Valid()) {
        randomZRTP(randBuf, RS_LENGTH);
        hmacFunction(randBuf, RS_LENGTH, (unsigned char*)initiator, strlen(initiator), rs2IDi, &macLen);
        hmacFunction(randBuf, RS_LENGTH, (unsigned char*)responder, strlen(responder), rs2IDr, &macLen);
    }
    else {
        rs2Valid = true;
        hmacFunction((unsigned char*)zidRec->getRs2(), RS_LENGTH, (unsigned char*)initiator, strlen(initiator), rs2IDi, &macLen);
        hmacFunction((unsigned char*)zidRec->getRs2(), RS_LENGTH, (unsigned char*)responder, strlen(responder), rs2IDr, &macLen);
        detailInfo.secretsCached |= Rs2;
    }

    /*
    * For the time being we don't support this type of shared secrect. Could be
    * easily done: somebody sets some data into our ZRtp object, check it here
    * and use it. Otherwise use the random data.
    */
    randomZRTP(randBuf, RS_LENGTH);
    hmacFunction(randBuf, RS_LENGTH, (unsigned char*)initiator, strlen(initiator), auxSecretIDi, &macLen);
    hmacFunction(randBuf, RS_LENGTH, (unsigned char*)responder, strlen(responder), auxSecretIDr, &macLen);

    if (!zidRec->isMITMKeyAvailable()) {
        randomZRTP(randBuf, RS_LENGTH);
        hmacFunction(randBuf, RS_LENGTH, (unsigned char*)initiator, strlen(initiator), pbxSecretIDi, &macLen);
        hmacFunction(randBuf, RS_LENGTH, (unsigned char*)responder, strlen(responder), pbxSecretIDr, &macLen);

    }
    else {
        hmacFunction((unsigned char*)zidRec->getMiTMData(), RS_LENGTH, (unsigned char*)initiator, strlen(initiator), pbxSecretIDi, &macLen);
        hmacFunction((unsigned char*)zidRec->getMiTMData(), RS_LENGTH, (unsigned char*)responder, strlen(responder), pbxSecretIDr, &macLen);
        detailInfo.secretsCached |= Pbx;
    }
}