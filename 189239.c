ZrtpPacketConfirm* ZRtp::prepareConfirm2MultiStream(ZrtpPacketConfirm* confirm1, uint32_t* errMsg) {

    // check Confirm1 packet using the keys
    // prepare Confirm2 packet
    // don't update SAS, RS
    sendInfo(Info, InfoInitConf1Received);

    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;

    closeHashCtx(msgShaContext, messageHash);
    msgShaContext = NULL;
    myRole = Initiator;

    generateKeysMultiStream();

    // Use the Responder's keys here because we are Initiator here and
    // receive packets from Responder
    int32_t hmlen = (confirm1->getLength() - 9) * ZRTP_WORD_SIZE;

    // Use negotiated HMAC (hash)
    hmacFunction(hmacKeyR, hashLength, (unsigned char*)confirm1->getHashH0(), hmlen, confMac, &macLen);

    if (memcmp(confMac, confirm1->getHmac(), HMAC_SIZE) != 0) {
        *errMsg = ConfirmHMACWrong;
        return NULL;
    }
    // Cast away the const for the IV - the standalone AES CFB modifies IV on return
    cipher->getDecrypt()(zrtpKeyR, cipher->getKeylen(), (uint8_t*)confirm1->getIv(), confirm1->getHashH0(), hmlen);

    // Because we are initiator the protocol engine didn't receive Commit and
    // because we are using multi-stream mode here we also did not receive a DHPart1 and
    // thus could not store a responder's H2 or H1. A two step hash is required to
    // re-compute H1, H2.
    // USe implicit hash function.
    uint8_t tmpHash[IMPL_MAX_DIGEST_LENGTH];
    hashFunctionImpl(confirm1->getHashH0(), HASH_IMAGE_SIZE, tmpHash); // Compute peer's H1 in tmpHash
    hashFunctionImpl(tmpHash, HASH_IMAGE_SIZE, tmpHash);               // Compute peer's H2 in tmpHash
    memcpy(peerH2, tmpHash, HASH_IMAGE_SIZE);                          // copy and truncate to peerH2

    // Check HMAC of previous Hello packet stored in temporary buffer. The
    // HMAC key of the Hello packet is peer's H2 that was computed above.
    // Refer to chapter 9.1 and chapter 10.
    if (!checkMsgHmac(peerH2)) {
        sendInfo(Severe, SevereHelloHMACFailed);
        *errMsg = CriticalSWError;
        return NULL;
    }
    // now generate my Confirm2 message
    zrtpConfirm2.setMessageType((uint8_t*)Confirm2Msg);
    zrtpConfirm2.setHashH0(H0);
    zrtpConfirm2.setExpTime(0xFFFFFFFF);
    zrtpConfirm2.setIv(randomIV);

    // Encrypt and HMAC with Initiator's key - we are Initiator here
    hmlen = (zrtpConfirm2.getLength() - 9) * ZRTP_WORD_SIZE;
    cipher->getEncrypt()(zrtpKeyI, cipher->getKeylen(), randomIV, zrtpConfirm2.getHashH0(), hmlen);

    // Use negotiated HMAC (hash)
    hmacFunction(hmacKeyI, hashLength, (unsigned char*)zrtpConfirm2.getHashH0(), hmlen, confMac, &macLen);

    zrtpConfirm2.setHmac(confMac);
    return &zrtpConfirm2;
}