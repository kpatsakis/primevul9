ZrtpPacketConfirm* ZRtp::prepareConfirm2(ZrtpPacketConfirm* confirm1, uint32_t* errMsg) {

    sendInfo(Info, InfoInitConf1Received);

    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;

    // Use the Responder's keys here because we are Initiator here and
    // receive packets from Responder
    int16_t hmlen = (confirm1->getLength() - 9) * ZRTP_WORD_SIZE;

    // Use negotiated HMAC (hash)
    hmacFunction(hmacKeyR, hashLength, (unsigned char*)confirm1->getHashH0(), hmlen, confMac, &macLen);

    if (memcmp(confMac, confirm1->getHmac(), HMAC_SIZE) != 0) {
        *errMsg = ConfirmHMACWrong;
        return NULL;
    }
    cipher->getDecrypt()(zrtpKeyR, cipher->getKeylen(), (uint8_t*)confirm1->getIv(), confirm1->getHashH0(), hmlen);

    // Check HMAC of DHPart1 packet stored in temporary buffer. The
    // HMAC key of the DHPart1 packet is peer's H0 that is contained in
    // Confirm1. Refer to chapter 9.
    if (!checkMsgHmac(confirm1->getHashH0())) {
        sendInfo(Severe, SevereDH1HMACFailed);
        *errMsg = CriticalSWError;
        return NULL;
    }
    signatureLength = confirm1->getSignatureLength();
    if (signSasSeen && signatureLength > 0) {
        signatureData = confirm1->getSignatureData();
        callback->checkSASSignature(sasHash);
        // TODO: error handling if checkSASSignature returns false.
    }
    /*
     * The Confirm1 is ok, handle the Retained secret stuff and inform
     * GUI about state.
     */
    bool sasFlag = confirm1->isSASFlag();

    // Our peer did not confirm the SAS in last session, thus reset
    // our SAS flag too. Reset the flag also if paranoidMode is true.
    if (!sasFlag || paranoidMode) {
        zidRec->resetSasVerified();
    }
    // get verified flag from current RS1 before set a new RS1. This
    // may not be set even if peer's flag is set in confirm1 message.
    sasFlag = zidRec->isSasVerified();

    // now we are ready to save the new RS1 which inherits the verified
    // flag from old RS1
    zidRec->setNewRs1((const uint8_t*)newRs1);

    // now generate my Confirm2 message
    zrtpConfirm2.setMessageType((uint8_t*)Confirm2Msg);
    zrtpConfirm2.setHashH0(H0);

    if (sasFlag) {
        zrtpConfirm2.setSASFlag();
    }
    zrtpConfirm2.setExpTime(0xFFFFFFFF);
    zrtpConfirm2.setIv(randomIV);

    // Compute PBX secret if we are in enrollemnt mode (PBX user agent)
    // or enrollment was enabled at normal user agent and flag in confirm packet
    if (enrollmentMode || (enableMitmEnrollment && confirm1->isPBXEnrollment())) {
        computePBXSecret();

        // if this runs at PBX user agent enrollment service then set flag in confirm
        // packet and store the MitM key. The PBX user agent service always stores
        // its MitM key.
        if (enrollmentMode) {
            // As clarification to RFC6189: store new PBX secret only if we don't have
            // a matching PBX secret for the peer's ZID.
            if (!peerIsEnrolled) {
                computePBXSecret();
                zidRec->setMiTMData(pbxSecretTmp);
            }
            // Set flag to enable user's client to ask for confirmation or re-confirmation.
            zrtpConfirm2.setPBXEnrollment();
        }
    }
    getZidCacheInstance()->saveRecord(zidRec);

    // Encrypt and HMAC with Initiator's key - we are Initiator here
    hmlen = (zrtpConfirm2.getLength() - 9) * ZRTP_WORD_SIZE;
    cipher->getEncrypt()(zrtpKeyI, cipher->getKeylen(), randomIV, zrtpConfirm2.getHashH0(), hmlen);

    // Use negotiated HMAC (hash)
    hmacFunction(hmacKeyI, hashLength, (unsigned char*)zrtpConfirm2.getHashH0(), hmlen, confMac, &macLen);

    zrtpConfirm2.setHmac(confMac);

    // Ask for enrollment only if enabled via configuration and the
    // confirm1 packet contains the enrollment flag. The enrolling user
    // agent stores the MitM key only if the user accepts the enrollment
    // request.
    if (enableMitmEnrollment && confirm1->isPBXEnrollment()) {
        // As clarification to RFC6189: if already enrolled (having a matching PBX secret)
        // ask for reconfirmation.
        if (!peerIsEnrolled) {
            callback->zrtpAskEnrollment(EnrollmentRequest);
        }
        else {
            callback->zrtpAskEnrollment(EnrollmentReconfirm);
        }
    }
    return &zrtpConfirm2;
}