ZrtpPacketCommit* ZRtp::prepareCommit(ZrtpPacketHello *hello, uint32_t* errMsg) {

    // Save data before detailed checks - may aid in analysing problems
    peerClientId.assign((char*)hello->getClientId(), ZRTP_WORD_SIZE * 4);
    memcpy(peerHelloVersion, hello->getVersion(), ZRTP_WORD_SIZE);
    peerHelloVersion[ZRTP_WORD_SIZE] = 0;

    // Save our peer's (presumably the Responder) ZRTP id
    memcpy(peerZid, hello->getZid(), ZID_SIZE);
    if (memcmp(peerZid, ownZid, ZID_SIZE) == 0) {       // peers have same ZID????
        *errMsg = EqualZIDHello;
        return NULL;
    }
    memcpy(peerH3, hello->getH3(), HASH_IMAGE_SIZE);

    int32_t helloLen = hello->getLength() * ZRTP_WORD_SIZE;

    // calculate hash over the received Hello packet - is peer's hello hash.
    // Use implicit hash algorithm
    hashFunctionImpl((unsigned char*)hello->getHeaderBase(), helloLen, peerHelloHash);

    sendInfo(Info, InfoHelloReceived);

    /*
     * The Following section extracts the algorithm from the peer's Hello
     * packet. Always the preferend offered algorithms are
     * used. If the received Hello does not contain algo specifiers
     * or offers only unsupported optional algos then replace
     * these with mandatory algos and put them into the Commit packet.
     * Refer to the findBest*() functions.
     * If this is a MultiStream ZRTP object then do not get the cipher,
     * authentication from hello packet but use the pre-initialized values
     * as proposed by the standard. If we switch to responder mode the
     * commit packet may contain other algos - see function
     * prepareConfirm2MultiStream(...).
     */
    sasType = findBestSASType(hello);

    if (!multiStream) {
        pubKey = findBestPubkey(hello);                 // Check for public key algorithm first, sets 'hash' as well
        if (hash == NULL) {
            *errMsg = UnsuppHashType;
            return NULL;
        }
        if (cipher == NULL)                             // public key selection may have set the cipher already
            cipher = findBestCipher(hello, pubKey);
        authLength = findBestAuthLen(hello);
        multiStreamAvailable = checkMultiStream(hello);
    }
    else {
        if (checkMultiStream(hello)) {
            return prepareCommitMultiStream(hello);
        }
        else {
            // we are in multi-stream but peer does not offer multi-stream
            // return error code to other party - unsupported PK, must be Mult
            *errMsg = UnsuppPKExchange;
            return NULL;
        }
    }
    setNegotiatedHash(hash);

    // Modify here when introducing new DH key agreement, for example
    // elliptic curves.
    dhContext = new ZrtpDH(pubKey->getName());
    dhContext->generatePublicKey();

    dhContext->getPubKeyBytes(pubKeyBytes);
    sendInfo(Info, InfoCommitDHGenerated);

    // Prepare IV data that we will use during confirm packet encryption.
    randomZRTP(randomIV, sizeof(randomIV));

    /*
     * Prepare our DHPart2 packet here. Required to compute HVI. If we stay
     * in Initiator role then we reuse this packet later in prepareDHPart2().
     * To create this DH packet we have to compute the retained secret ids,
     * thus get our peer's retained secret data first.
     */
    zidRec = getZidCacheInstance()->getRecord(peerZid);

    //Compute the Initator's and Responder's retained secret ids.
    computeSharedSecretSet(zidRec);

    // Check if a PBX application set the MitM flag.
    mitmSeen = hello->isMitmMode();

    signSasSeen = hello->isSasSign();
    // Construct a DHPart2 message (Initiator's DH message). This packet
    // is required to compute the HVI (Hash Value Initiator), refer to
    // chapter 5.4.1.1.

    // Fill the values in the DHPart2 packet
    zrtpDH2.setPubKeyType(pubKey->getName());
    zrtpDH2.setMessageType((uint8_t*)DHPart2Msg);
    zrtpDH2.setRs1Id(rs1IDi);
    zrtpDH2.setRs2Id(rs2IDi);
    zrtpDH2.setAuxSecretId(auxSecretIDi);
    zrtpDH2.setPbxSecretId(pbxSecretIDi);
    zrtpDH2.setPv(pubKeyBytes);
    zrtpDH2.setH1(H1);

    int32_t len = zrtpDH2.getLength() * ZRTP_WORD_SIZE;

    // Compute HMAC over DH2, excluding the HMAC field (HMAC_SIZE)
    // and store in DH2. Key to HMAC is H0, use HASH_IMAGE_SIZE bytes only.
    // Must use implicit HMAC functions.
    uint8_t hmac[IMPL_MAX_DIGEST_LENGTH];
    uint32_t macLen;
    hmacFunctionImpl(H0, HASH_IMAGE_SIZE, (uint8_t*)zrtpDH2.getHeaderBase(), len-(HMAC_SIZE), hmac, &macLen);
    zrtpDH2.setHMAC(hmac);

    // Compute the HVI, refer to chapter 5.4.1.1 of the specification
    computeHvi(&zrtpDH2, hello);

    zrtpCommit.setZid(ownZid);
    zrtpCommit.setHashType((uint8_t*)hash->getName());
    zrtpCommit.setCipherType((uint8_t*)cipher->getName());
    zrtpCommit.setAuthLen((uint8_t*)authLength->getName());
    zrtpCommit.setPubKeyType((uint8_t*)pubKey->getName());
    zrtpCommit.setSasType((uint8_t*)sasType->getName());
    zrtpCommit.setHvi(hvi);
    zrtpCommit.setH2(H2);

    len = zrtpCommit.getLength() * ZRTP_WORD_SIZE;

    // Compute HMAC over Commit, excluding the HMAC field (HMAC_SIZE)
    // and store in Hello. Key to HMAC is H1, use HASH_IMAGE_SIZE bytes only.
    // Must use implicit HMAC functions.
    hmacFunctionImpl(H1, HASH_IMAGE_SIZE, (uint8_t*)zrtpCommit.getHeaderBase(), len-(HMAC_SIZE), hmac, &macLen);
    zrtpCommit.setHMAC(hmac);

    // hash first messages to produce overall message hash
    // First the Responder's Hello message, second the Commit (always Initator's).
    // Must use negotiated hash.
    msgShaContext = createHashCtx();
    hashCtxFunction(msgShaContext, (unsigned char*)hello->getHeaderBase(), helloLen);
    hashCtxFunction(msgShaContext, (unsigned char*)zrtpCommit.getHeaderBase(), len);

    // store Hello data temporarily until we can check HMAC after receiving Commit as
    // Responder or DHPart1 as Initiator
    storeMsgTemp(hello);

    return &zrtpCommit;
}