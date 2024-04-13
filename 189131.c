ZrtpPacketConfirm* ZRtp::prepareConfirm1(ZrtpPacketDHPart* dhPart2, uint32_t* errMsg) {

    uint8_t* pvi;

    sendInfo(Info, InfoRespDH2Received);

    // Because we are responder we received a Commit and stored its H2.
    // Now re-compute H2 from received H1 and compare with stored peer's H2.
    // Use implicit hash function
    uint8_t tmpHash[IMPL_MAX_DIGEST_LENGTH];
    hashFunctionImpl(dhPart2->getH1(), HASH_IMAGE_SIZE, tmpHash);
    if (memcmp(tmpHash, peerH2, HASH_IMAGE_SIZE) != 0) {
        *errMsg = IgnorePacket;
        return NULL;
    }

    // Check HMAC of Commit packet stored in temporary buffer. The
    // HMAC key of the Commit packet is peer's H1 that is contained in
    // DHPart2. Refer to chapter 9.1 and chapter 10.
    if (!checkMsgHmac(dhPart2->getH1())) {
        sendInfo(Severe, SevereCommitHMACFailed);
        *errMsg = CriticalSWError;
        return NULL;
    }
    // Now we have the peer's pvi. Because we are responder re-compute my hvi
    // using my Hello packet and the Initiator's DHPart2 and compare with
    // hvi sent in commit packet. If it doesn't macht then a MitM attack
    // may have occured.
    computeHvi(dhPart2, currentHelloPacket);
    if (memcmp(hvi, peerHvi, HVI_SIZE) != 0) {
        *errMsg = DHErrorWrongHVI;
        return NULL;
    }
    DHss = new uint8_t[dhContext->getDhSize()];
    if (DHss == NULL) {
        *errMsg = CriticalSWError;
        return NULL;
    }
    // Get and check the Initiator's public value, see chap. 5.4.2 of the spec
    pvi = dhPart2->getPv();
    if (!dhContext->checkPubKey(pvi)) {
        *errMsg = DHErrorWrongPV;
        return NULL;
    }
    dhContext->computeSecretKey(pvi, DHss);
    // Hash the Initiator's DH2 into the message Hash (other messages already
    // prepared, see method prepareDHPart1().
    // Use neotiated hash function
    hashCtxFunction(msgShaContext, (unsigned char*)dhPart2->getHeaderBase(), dhPart2->getLength() * ZRTP_WORD_SIZE);

    closeHashCtx(msgShaContext, messageHash);
    msgShaContext = NULL;
    /*
     * The expected shared secret Ids were already computed when we built the
     * DHPart1 packet. Generate s0, all depended keys, and the new RS1 value
     * for the ZID record. The functions also performs sign SAS callback if it's
     * active. May reset the verify flag in ZID record.
     */
    generateKeysResponder(dhPart2, zidRec);

    delete dhContext;
    dhContext = NULL;

    // Fill in Confirm1 packet.
    zrtpConfirm1.setMessageType((uint8_t*)Confirm1Msg);

    // Check if user verfied the SAS in a previous call and thus verfied
    // the retained secret. Don't set the verified flag if paranoidMode is true.
    if (zidRec->isSasVerified() && !paranoidMode) {
        zrtpConfirm1.setSASFlag();
    }
    zrtpConfirm1.setExpTime(0xFFFFFFFF);
    zrtpConfirm1.setIv(randomIV);
    zrtpConfirm1.setHashH0(H0);

    // if this runs at PBX user agent enrollment service then set flag in confirm
    // packet and store the MitM key
    if (enrollmentMode) {
        // As clarification to RFC6189: store new PBX secret only if we don't have
        // a matching PBX secret for the peer's ZID.
        if (!peerIsEnrolled) {
            computePBXSecret();
            zidRec->setMiTMData(pbxSecretTmp);
        }
        // Set flag to enable user's client to ask for confirmation or re-confirmation.
        zrtpConfirm1.setPBXEnrollment();
    }
    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;

    // Encrypt and HMAC with Responder's key - we are Respondere here
    int hmlen = (zrtpConfirm1.getLength() - 9) * ZRTP_WORD_SIZE;
    cipher->getEncrypt()(zrtpKeyR, cipher->getKeylen(), randomIV, zrtpConfirm1.getHashH0(), hmlen);
    hmacFunction(hmacKeyR, hashLength, (unsigned char*)zrtpConfirm1.getHashH0(), hmlen, confMac, &macLen);

    zrtpConfirm1.setHmac(confMac);

    // store DHPart2 data temporarily until we can check HMAC after receiving Confirm2
    storeMsgTemp(dhPart2);
    return &zrtpConfirm1;
}