bool ZRtp::sendSASRelayPacket(uint8_t* sh, std::string render) {

    uint8_t confMac[MAX_DIGEST_LENGTH];
    uint32_t macLen;
    uint8_t* hkey, *ekey;

    // If we are responder then the PBX used it's Initiator keys
    if (myRole == Responder) {
        hkey = hmacKeyR;
        ekey = zrtpKeyR;
        // TODO: check signature length in zrtpConfirm1 and if not zero copy Signature data
    }
    else {
        hkey = hmacKeyI;
        ekey = zrtpKeyI;
        // TODO: check signature length in zrtpConfirm2 and if not zero copy Signature data
    }
    // Prepare IV data that we will use during confirm packet encryption.
    randomZRTP(randomIV, sizeof(randomIV));
    zrtpSasRelay.setIv(randomIV);
    zrtpSasRelay.setTrustedSas(sh);
    zrtpSasRelay.setSasAlgo((uint8_t*)render.c_str());

    int16_t hmlen = (zrtpSasRelay.getLength() - 9) * ZRTP_WORD_SIZE;
    cipher->getEncrypt()(ekey, cipher->getKeylen(), randomIV, (uint8_t*)zrtpSasRelay.getFiller(), hmlen);

    // Use negotiated HMAC (hash)
    hmacFunction(hkey, hashLength, (unsigned char*)zrtpSasRelay.getFiller(), hmlen, confMac, &macLen);

    zrtpSasRelay.setHmac(confMac);

    stateEngine->sendSASRelay(&zrtpSasRelay);
    return true;
}