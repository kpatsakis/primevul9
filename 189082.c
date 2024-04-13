void ZRtp::generateKeysInitiator(ZrtpPacketDHPart *dhPart, ZIDRecord *zidRec) {
    const uint8_t* setD[3];
    int32_t rsFound = 0;

    setD[0] = setD[1] = setD[2] = NULL;

    detailInfo.secretsMatchedDH = 0;
    if (memcmp(rs1IDr, dhPart->getRs1Id(), HMAC_SIZE) == 0 || memcmp(rs1IDr, dhPart->getRs2Id(), HMAC_SIZE) == 0)
        detailInfo.secretsMatchedDH |= Rs1;
    if (memcmp(rs2IDr, dhPart->getRs1Id(), HMAC_SIZE) == 0 || memcmp(rs2IDr, dhPart->getRs2Id(), HMAC_SIZE) == 0)
        detailInfo.secretsMatchedDH |= Rs2;
    /*
     * Select the real secrets into setD. The dhPart is DHpart1 message
     * received from responder. rs1IDr and rs2IDr are the expected ids using
     * the initator's cached retained secrets.
     */
    // Check which RS we shall use for first place (s1)
    detailInfo.secretsMatched = 0;
    if (memcmp(rs1IDr, dhPart->getRs1Id(), HMAC_SIZE) == 0) {
        setD[0] = zidRec->getRs1();
        rsFound = 0x1;
        detailInfo.secretsMatched = Rs1;
    }
    else if (memcmp(rs1IDr, dhPart->getRs2Id(), HMAC_SIZE) == 0) {
        setD[0] = zidRec->getRs1();
        rsFound = 0x2;
        detailInfo.secretsMatched = Rs1;
    }
    else if (memcmp(rs2IDr, dhPart->getRs1Id(), HMAC_SIZE) == 0) {
        setD[0] = zidRec->getRs2();
        rsFound = 0x4;
        detailInfo.secretsMatched = Rs2;
    }
    else if (memcmp(rs2IDr, dhPart->getRs2Id(), HMAC_SIZE) == 0) {
        setD[0] = zidRec->getRs2();
        rsFound = 0x8;
        detailInfo.secretsMatched = Rs2;
    }
    /* *** Not yet supported
    if (memcmp(auxSecretIDr, dhPart->getAuxSecretId(), 8) == 0) {
    DEBUGOUT((fprintf(stdout, "%c: Match for aux secret found\n", zid[0])));
        setD[1] = auxSecret;
    }
    */
    // check if we have a matching PBX secret and place it third (s3)
    if (memcmp(pbxSecretIDr, dhPart->getPbxSecretId(), HMAC_SIZE) == 0) {
        DEBUGOUT((fprintf(stdout, "%c: Match for Other_secret found\n", zid[0])));
        setD[2] = zidRec->getMiTMData();
        detailInfo.secretsMatched |= Pbx;
        detailInfo.secretsMatchedDH |= Pbx;
        // Flag to record that fact that we have a MitM key of the other peer.
        peerIsEnrolled = true;
    }
    // Check if some retained secrets found
    if (rsFound == 0) {                        // no RS matches found
        if (rs1Valid || rs2Valid) {            // but valid RS records in cache
            sendInfo(Warning, WarningNoExpectedRSMatch);
            zidRec->resetSasVerified();
        }
        else {                                 // No valid RS record in cache
            sendInfo(Warning, WarningNoRSMatch);
        }
    }
    else {                                     // at least one RS matches
        sendInfo(Info, InfoRSMatchFound);
    }
    /*
     * Ready to generate s0 here.
     * The formular to compute S0 (Refer to ZRTP specification 5.4.4):
     *
      s0 = hash( counter | DHResult | "ZRTP-HMAC-KDF" | ZIDi | ZIDr | \
      total_hash | len(s1) | s1 | len(s2) | s2 | len(s3) | s3)
     *
     * Note: in this function we are Initiator, thus ZIDi is our zid
     * (zid), ZIDr is the peer's zid (peerZid).
     */

    /*
     * These arrays hold the pointers and lengths of the data that must be
     * hashed to create S0.  According to the formula the max number of
     * elements to hash is 12, add one for the terminating "NULL"
     */
    unsigned char* data[13];
    unsigned int   length[13];
    uint32_t pos = 0;                  // index into the array

    // we need a number of length data items, so define them here
    uint32_t counter, sLen[3];

    //Very first element is a fixed counter, big endian
    counter = 1;
    counter = zrtpHtonl(counter);
    data[pos] = (unsigned char*)&counter;
    length[pos++] = sizeof(uint32_t);

    // Next is the DH result itself
    data[pos] = DHss;
    length[pos++] = dhContext->getDhSize();

    // Next the fixed string "ZRTP-HMAC-KDF"
    data[pos] = (unsigned char*)KDFString;
    length[pos++] = strlen(KDFString);

    // Next is Initiator's id (ZIDi), in this case as Initiator
    // it is zid
    data[pos] = ownZid;
    length[pos++] = ZID_SIZE;

    // Next is Responder's id (ZIDr), in this case our peer's id
    data[pos] = peerZid;
    length[pos++] = ZID_SIZE;

    // Next ist total hash (messageHash) itself
    data[pos] = messageHash;
    length[pos++] = hashLength;

    /*
     * For each matching shared secret hash the length of
     * the shared secret as 32 bit big-endian number followd by the
     * shared secret itself. The length of a shared seceret is
     * currently fixed to RS_LENGTH. If a shared
     * secret is not used _only_ its length is hased as zero
     * length. NOTE: if implementing auxSecret and/or pbxSecret -> check
     * this length stuff again.
     */
    int secretHashLen = RS_LENGTH;
    secretHashLen = zrtpHtonl(secretHashLen);        // prepare 32 bit big-endian number

    for (int32_t i = 0; i < 3; i++) {
        if (setD[i] != NULL) {           // a matching secret, set length, then secret
            sLen[i] = secretHashLen;
            data[pos] = (unsigned char*)&sLen[i];
            length[pos++] = sizeof(uint32_t);
            data[pos] = (unsigned char*)setD[i];
            length[pos++] = RS_LENGTH;
        }
        else {                           // no machting secret, set length 0, skip secret
            sLen[i] = 0;
            data[pos] = (unsigned char*)&sLen[i];
            length[pos++] = sizeof(uint32_t);
        }
    }

    data[pos] = NULL;
    hashListFunction(data, length, s0);
//  hexdump("S0 I", s0, hashLength);

    memset(DHss, 0, dhContext->getDhSize());
    delete[] DHss;
    DHss = NULL;

    computeSRTPKeys();
    memset(s0, 0, MAX_DIGEST_LENGTH);
}