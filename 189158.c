ZrtpPacketDHPart* ZRtp::prepareDHPart1(ZrtpPacketCommit *commit, uint32_t* errMsg) {

    sendInfo(Info, InfoRespCommitReceived);

    // The following code check the hash chain according chapter 10 to detect
    // false ZRTP packets.
    // Must use the implicit hash function.
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

    // check if we support the commited Cipher type
    AlgorithmEnum* cp = &zrtpSymCiphers.getByName((const char*)commit->getCipherType());
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
        // Compute the Initator's and Responder's retained secret ids
        // with the committed hash.
        computeSharedSecretSet(zidRec);
    }
    // check if we support the commited pub key type
    cp = &zrtpPubKeys.getByName((const char*)commit->getPubKeysType());
    if (!cp->isValid()) { // no match - something went wrong
        *errMsg = UnsuppPKExchange;
        return NULL;
    }
    if (*(int32_t*)(cp->getName()) == *(int32_t*)ec38) {
        if (*(int32_t*)(hash->getName()) != *(int32_t*)s384) {
            *errMsg = UnsuppHashType;
            return NULL;
        }
    }
    pubKey = cp;

    // check if we support the commited SAS type
    cp = &zrtpSasTypes.getByName((const char*)commit->getSasType());
    if (!cp->isValid()) { // no match - something went wrong
        *errMsg = UnsuppSASScheme;
        return NULL;
    }
    sasType = cp;

    // dhContext cannot be NULL - always setup during prepareCommit()
    // check if we can use the dhContext prepared by prepareCOmmit(),
    // if not delete old DH context and generate new one
    // The algorithm names are 4 chars only, thus we can cast to int32_t
    if (*(int32_t*)(dhContext->getDHtype()) != *(int32_t*)(pubKey->getName())) {
        delete dhContext;
        dhContext = new ZrtpDH(pubKey->getName());
        dhContext->generatePublicKey();
    }
    sendInfo(Info, InfoDH1DHGenerated);

    dhContext->getPubKeyBytes(pubKeyBytes);

    // Setup a DHPart1 packet.
    zrtpDH1.setPubKeyType(pubKey->getName());
    zrtpDH1.setMessageType((uint8_t*)DHPart1Msg);
    zrtpDH1.setRs1Id(rs1IDr);
    zrtpDH1.setRs2Id(rs2IDr);
    zrtpDH1.setAuxSecretId(auxSecretIDr);
    zrtpDH1.setPbxSecretId(pbxSecretIDr);
    zrtpDH1.setPv(pubKeyBytes);
    zrtpDH1.setH1(H1);

    int32_t len = zrtpDH1.getLength() * ZRTP_WORD_SIZE;

    // Compute HMAC over DHPart1, excluding the HMAC field (HMAC_SIZE)
    // and store in DHPart1.
    // Use implicit Hash function
    uint8_t hmac[IMPL_MAX_DIGEST_LENGTH];
    uint32_t macLen;
    hmacFunctionImpl(H0, HASH_IMAGE_SIZE, (uint8_t*)zrtpDH1.getHeaderBase(), len-(HMAC_SIZE), hmac, &macLen);
    zrtpDH1.setHMAC(hmac);

    // We are definitly responder. Save the peer's hvi for later compare.
    myRole = Responder;
    memcpy(peerHvi, commit->getHvi(), HVI_SIZE);

    // We are responder. Release a possibly pre-computed SHA context
    // because this was prepared for Initiator. Then create a new one.
    if (msgShaContext != NULL) {
        closeHashCtx(msgShaContext, NULL);
    }
    msgShaContext = createHashCtx();

    // Hash messages to produce overall message hash:
    // First the Responder's (my) Hello message, second the Commit
    // (always Initator's), then the DH1 message (which is always a
    // Responder's message).
    // Must use negotiated hash
    hashCtxFunction(msgShaContext, (unsigned char*)currentHelloPacket->getHeaderBase(), currentHelloPacket->getLength() * ZRTP_WORD_SIZE);
    hashCtxFunction(msgShaContext, (unsigned char*)commit->getHeaderBase(), commit->getLength() * ZRTP_WORD_SIZE);
    hashCtxFunction(msgShaContext, (unsigned char*)zrtpDH1.getHeaderBase(), zrtpDH1.getLength() * ZRTP_WORD_SIZE);

    // store Commit data temporarily until we can check HMAC after we got DHPart2
    storeMsgTemp(commit);

    return &zrtpDH1;
}