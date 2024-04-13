ZrtpPacketDHPart* ZRtp::prepareDHPart2(ZrtpPacketDHPart *dhPart1, uint32_t* errMsg) {

    uint8_t* pvr;

    sendInfo(Info, InfoInitDH1Received);

    // Because we are initiator the protocol engine didn't receive Commit
    // thus could not store a peer's H2. A two step SHA256 is required to
    // re-compute H3. Then compare with peer's H3 from peer's Hello packet.
    // Must use implicit hash function.
    uint8_t tmpHash[IMPL_MAX_DIGEST_LENGTH];
    hashFunctionImpl(dhPart1->getH1(), HASH_IMAGE_SIZE, tmpHash); // Compute peer's H2
    memcpy(peerH2, tmpHash, HASH_IMAGE_SIZE);
    hashFunctionImpl(peerH2, HASH_IMAGE_SIZE, tmpHash);          // Compute peer's H3 (tmpHash)

    if (memcmp(tmpHash, peerH3, HASH_IMAGE_SIZE) != 0) {
        *errMsg = IgnorePacket;
        return NULL;
    }

    // Check HMAC of previous Hello packet stored in temporary buffer. The
    // HMAC key of the Hello packet is peer's H2 that was computed above.
    // Refer to chapter 9.1 and chapter 10.
    if (!checkMsgHmac(peerH2)) {
        sendInfo(Severe, SevereHelloHMACFailed);
        *errMsg = CriticalSWError;
        return NULL;
    }

    // get memory to store DH result TODO: make it fixed memory
    DHss = new uint8_t[dhContext->getDhSize()];
    if (DHss == NULL) {
        *errMsg = CriticalSWError;
        return NULL;
    }

    // get and check Responder's public value, see chap. 5.4.3 in the spec
    pvr = dhPart1->getPv();
    if (!dhContext->checkPubKey(pvr)) {
        *errMsg = DHErrorWrongPV;
        return NULL;
    }
    dhContext->computeSecretKey(pvr, DHss);

    myRole = Initiator;

    // We are Initiator: the Responder's Hello and the Initiator's (our) Commit
    // are already hashed in the context. Now hash the Responder's DH1 and then
    // the Initiator's (our) DH2 in that order.
    // Use the negotiated hash function.
    hashCtxFunction(msgShaContext, (unsigned char*)dhPart1->getHeaderBase(), dhPart1->getLength() * ZRTP_WORD_SIZE);
    hashCtxFunction(msgShaContext, (unsigned char*)zrtpDH2.getHeaderBase(), zrtpDH2.getLength() * ZRTP_WORD_SIZE);

    // Compute the message Hash
    closeHashCtx(msgShaContext, messageHash);
    msgShaContext = NULL;
    // Now compute the S0, all dependend keys and the new RS1. The function
    // also performs sign SAS callback if it's active.
    generateKeysInitiator(dhPart1, zidRec);

    delete dhContext;
    dhContext = NULL;

    // TODO: at initiator we can call signSAS at this point, don't dealy until confirm1 reveived
    // store DHPart1 data temporarily until we can check HMAC after receiving Confirm1
    storeMsgTemp(dhPart1);
    return &zrtpDH2;
}