ZrtpPacketConfirm* ZRtp::prepareConfirm1MultiStream(ZrtpPacketCommit* commit, uint32_t* errMsg) {

    sendInfo(Info, InfoRespCommitReceived);

    // The following code checks the hash chain according chapter 10 to detect
    // false ZRTP packets.
    // Use implicit hash function
    uint8_t tmpH3[IMPL_MAX_DIGEST_LENGTH];
    memcpy(peerH2, commit->getH2(), HASH_IMAGE_SIZE);
    hashFunctionImpl(peerH2, HASH_IMAGE_SIZE, tmpH3);

    if (memcmp(tmpH3, peerH3, HASH_IMAGE_SIZE) != 0) {
        *errMsg = IgnorePacket;
        return NULL;
    }

    // Check HMAC of previous Hello packet stored in temporary buffer. The
    // HMAC key of peer's Hello packet is peer's H2 that is contained in the
    // Commit packet. Refer to chapter 9.1.
    if (!checkMsgHmac(peerH2)) {
        sendInfo(Severe, SevereHelloHMACFailed);
        *errMsg = CriticalSWError;
        return NULL;
    }

    // check if Commit contains "Mult" as pub key type
    AlgorithmEnum* cp = &zrtpPubKeys.getByName((const char*)commit->getPubKeysType());
    if (!cp->isValid() || *(int32_t*)(cp->getName()) != *(int32_t*)mult) {
        *errMsg = UnsuppPKExchange;
        return NULL;
    }

    // check if we support the commited cipher
    cp = &zrtpSymCiphers.getByName((const char*)commit->getCipherType());
    if (!cp->isValid()) { // no match - something went wrong
        *errMsg = UnsuppCiphertype;
        return NULL;
    }
    cipher = cp;

    // check if we support the commited Authentication length
    cp = &zrtpAuthLengths.getByName((const char*)commit->getAuthLen());
    if (!cp->isValid()) { // no match - something went wrong
        *errMsg = UnsuppSRTPAuthTag;
        return NULL;
    }
    authLength = cp;

    // check if we support the commited hash type
    cp = &zrtpHashes.getByName((const char*)commit->getHashType());
    if (!cp->isValid()) { // no match - something went wrong
        *errMsg = UnsuppHashType;
        return NULL;
    }
    // check if the peer's commited hash is the same that we used when
    // preparing our commit packet. If not do the necessary resets and
    // recompute some data.
    if (*(int32_t*)(hash->getName()) != *(int32_t*)(cp->getName())) {
        hash = cp;
        setNegotiatedHash(hash);
    }
    myRole = Responder;

    // We are responder. Release a possibly pre-computed SHA256 context
    // because this was prepared for Initiator. Then create a new one.
    if (msgShaContext != NULL) {
        closeHashCtx(msgShaContext, NULL);
    }
    msgShaContext = createHashCtx();

    // Hash messages to produce overall message hash:
    // First the Responder's (my) Hello message, second the Commit
    // (always Initator's)
    // use negotiated hash
    hashCtxFunction(msgShaContext, (unsigned char*)currentHelloPacket->getHeaderBase(), currentHelloPacket->getLength() * ZRTP_WORD_SIZE);
    hashCtxFunction(msgShaContext, (unsigned char*)commit->getHeaderBase(), commit->getLength() * ZRTP_WORD_SIZE);

    closeHashCtx(msgShaContext, messageHash);
    msgShaContext = NULL;

    generateKeysMultiStream();

    // Fill in Confirm1 packet.
    zrtpConfirm1.setMessageType((uint8_t*)Confirm1Msg);
    zrtpConfirm1.setExpTime(0xFFFFFFFF);
    zrtpConfirm1.setIv(randomIV);
    zrtpConfirm1.setHashH0(H0);

    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;

    // Encrypt and HMAC with Responder's key - we are Respondere here
    int32_t hmlen = (zrtpConfirm1.getLength() - 9) * ZRTP_WORD_SIZE;
    cipher->getEncrypt()(zrtpKeyR, cipher->getKeylen(), randomIV, zrtpConfirm1.getHashH0(), hmlen);

    // Use negotiated HMAC (hash)
    hmacFunction(hmacKeyR, hashLength, (unsigned char*)zrtpConfirm1.getHashH0(), hmlen, confMac, &macLen);

    zrtpConfirm1.setHmac(confMac);

    // Store Commit data temporarily until we can check HMAC after receiving Confirm2
    storeMsgTemp(commit);
    return &zrtpConfirm1;
}