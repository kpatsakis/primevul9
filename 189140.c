ZrtpPacketCommit* ZRtp::prepareCommitMultiStream(ZrtpPacketHello *hello) {

    randomZRTP(hvi, ZRTP_WORD_SIZE*4);  // This is the Multi-Stream NONCE size

    zrtpCommit.setZid(ownZid);
    zrtpCommit.setHashType((uint8_t*)hash->getName());
    zrtpCommit.setCipherType((uint8_t*)cipher->getName());
    zrtpCommit.setAuthLen((uint8_t*)authLength->getName());
    zrtpCommit.setPubKeyType((uint8_t*)mult);  // this is fixed because of Multi Stream mode
    zrtpCommit.setSasType((uint8_t*)sasType->getName());
    zrtpCommit.setNonce(hvi);
    zrtpCommit.setH2(H2);

    int32_t len = zrtpCommit.getLength() * ZRTP_WORD_SIZE;

    // Compute HMAC over Commit, excluding the HMAC field (HMAC_SIZE)
    // and store in Hello. Key to HMAC is H1, use HASH_IMAGE_SIZE bytes only.
    // Must use the implicit HMAC function.
    uint8_t hmac[IMPL_MAX_DIGEST_LENGTH];
    uint32_t macLen;
    hmacFunctionImpl(H1, HASH_IMAGE_SIZE, (uint8_t*)zrtpCommit.getHeaderBase(), len-(HMAC_SIZE), hmac, &macLen);
    zrtpCommit.setHMACMulti(hmac);


    // hash first messages to produce overall message hash
    // First the Responder's Hello message, second the Commit
    // (always Initator's).
    // Must use the negotiated hash.
    msgShaContext = createHashCtx();

    int32_t helloLen = hello->getLength() * ZRTP_WORD_SIZE;
    hashCtxFunction(msgShaContext, (unsigned char*)hello->getHeaderBase(), helloLen);
    hashCtxFunction(msgShaContext, (unsigned char*)zrtpCommit.getHeaderBase(), len);

    // store Hello data temporarily until we can check HMAC after receiving Commit as
    // Responder or DHPart1 as Initiator
    storeMsgTemp(hello);

    return &zrtpCommit;
}