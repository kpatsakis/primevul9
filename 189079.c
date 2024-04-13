ZrtpPacketConf2Ack* ZRtp::prepareConf2Ack(ZrtpPacketConfirm *confirm2, uint32_t* errMsg) {

    sendInfo(Info, InfoRespConf2Received);

    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;

    // Use the Initiator's keys here because we are Responder here and
    // reveice packets from Initiator
    int16_t hmlen = (confirm2->getLength() - 9) * ZRTP_WORD_SIZE;

    // Use negotiated HMAC (hash)
    hmacFunction(hmacKeyI, hashLength,
                 (unsigned char*)confirm2->getHashH0(),
                 hmlen, confMac, &macLen);

    if (memcmp(confMac, confirm2->getHmac(), HMAC_SIZE) != 0) {
        *errMsg = ConfirmHMACWrong;
        return NULL;
    }
    // Cast away the const for the IV - the standalone AES CFB modifies IV on return
    cipher->getDecrypt()(zrtpKeyI, cipher->getKeylen(), (uint8_t*)confirm2->getIv(), confirm2->getHashH0(), hmlen);

    if (!multiStream) {
        // Check HMAC of DHPart2 packet stored in temporary buffer. The
        // HMAC key of the DHPart2 packet is peer's H0 that is contained in
        // Confirm2. Refer to chapter 9.1 and chapter 10.
        if (!checkMsgHmac(confirm2->getHashH0())) {
            sendInfo(Severe, SevereDH2HMACFailed);
            *errMsg = CriticalSWError;
            return NULL;
        }
        signatureLength = confirm2->getSignatureLength();
        if (signSasSeen && signatureLength > 0) {
            signatureData = confirm2->getSignatureData();
            callback->checkSASSignature(sasHash);
            // TODO: error handling if checkSASSignature returns false.
        }
        /*
        * The Confirm2 is ok, handle the Retained secret stuff and inform
        * GUI about state.
        */
        bool sasFlag = confirm2->isSASFlag();
        // Our peer did not confirm the SAS in last session, thus reset
        // our SAS flag too. Reset the flag also if paranoidMode is true.
        if (!sasFlag || paranoidMode) {
            zidRec->resetSasVerified();
        }

        // save new RS1, this inherits the verified flag from old RS1
        zidRec->setNewRs1((const uint8_t*)newRs1);
        getZidCacheInstance()->saveRecord(zidRec);

        // Ask for enrollment only if enabled via configuration and the
        // confirm packet contains the enrollment flag. The enrolling user
        // agent stores the MitM key only if the user accepts the enrollment
        // request.
        if (enableMitmEnrollment && confirm2->isPBXEnrollment()) {
            computePBXSecret();
            // As clarification to RFC6189: if already enrolled (having a matching PBX secret)
            // ask for reconfirmation.
            if (!peerIsEnrolled) {
                callback->zrtpAskEnrollment(EnrollmentRequest);
            }
            else {
                callback->zrtpAskEnrollment(EnrollmentReconfirm);
            }
        }
    }
    else {
        // Check HMAC of Commit packet stored in temporary buffer. The
        // HMAC key of the Commit packet is initiator's H1
        // use implicit hash function.
        uint8_t tmpHash[IMPL_MAX_DIGEST_LENGTH];
        hashFunctionImpl(confirm2->getHashH0(), HASH_IMAGE_SIZE, tmpHash); // Compute initiator's H1 in tmpHash

        if (!checkMsgHmac(tmpHash)) {
            sendInfo(Severe, SevereCommitHMACFailed);
            *errMsg = CriticalSWError;
            return NULL;
        }
    }
    return &zrtpConf2Ack;
}