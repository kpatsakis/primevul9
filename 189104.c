ZrtpPacketRelayAck* ZRtp::prepareRelayAck(ZrtpPacketSASrelay* srly, uint32_t* errMsg) {
    // handle and render SAS relay data only if the peer announced that it is a trusted
    // PBX. Don't handle SAS relay in paranoidMode.
    if (!mitmSeen || paranoidMode)
        return &zrtpRelayAck;

    uint8_t* hkey, *ekey;
    // If we are responder then the PBX used it's Initiator keys
    if (myRole == Responder) {
        hkey = hmacKeyI;
        ekey = zrtpKeyI;
    }
    else {
        hkey = hmacKeyR;
        ekey = zrtpKeyR;
    }

    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;

    int16_t hmlen = (srly->getLength() - 9) * ZRTP_WORD_SIZE;

    // Use negotiated HMAC (hash)
    hmacFunction(hkey, hashLength, (unsigned char*)srly->getFiller(), hmlen, confMac, &macLen);

    if (memcmp(confMac, srly->getHmac(), HMAC_SIZE) != 0) {
        *errMsg = ConfirmHMACWrong;
        return NULL;                // TODO - check error handling
    }
    // Cast away the const for the IV - the standalone AES CFB modifies IV on return
    cipher->getDecrypt()(ekey, cipher->getKeylen(), (uint8_t*)srly->getIv(), (uint8_t*)srly->getFiller(), hmlen);

    const uint8_t* newSasHash = srly->getTrustedSas();
    bool sasHashNull = true;
    for (int i = 0; i < HASH_IMAGE_SIZE; i++) {
        if (newSasHash[i] != 0) {
            sasHashNull = false;
            break;
        }
    }
    std::string cs(cipher->getReadable());
    cs.append("/").append(pubKey->getName());

    // Check if new SAS is null or a trusted MitM relationship doesn't exist.
    // If this is the case then don't render and don't show the new SAS - use
    // our computed SAS hash but we may use a different SAS rendering algorithm to
    // render the computed SAS.
    if (sasHashNull || !peerIsEnrolled) {
        cs.append("/MitM");
        newSasHash = sasHash;
    }
    else {
        cs.append("/SASviaMitM");
    }
    // If other SAS schemes required - check here and use others
    const uint8_t* render = srly->getSasAlgo();
    AlgorithmEnum* renderAlgo = &zrtpSasTypes.getByName((const char*)render);
    uint8_t sasBytes[4];
    if (renderAlgo->isValid()) {
        sasBytes[0] = newSasHash[0];
        sasBytes[1] = newSasHash[1];
        sasBytes[2] = newSasHash[2] & 0xf0;
        sasBytes[3] = 0;
        if (*(int32_t*)b32 == *(int32_t*)(renderAlgo->getName())) {
            SAS = Base32(sasBytes, 20).getEncoded();
        }
        else {
            SAS.assign(sas256WordsEven[sasBytes[0]]).append(":").append(sas256WordsOdd[sasBytes[1]]);
        }
    }
    bool verify = zidRec->isSasVerified() && srly->isSASFlag();
    callback->srtpSecretsOn(cs, SAS, verify);
    return &zrtpRelayAck;
}