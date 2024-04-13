void ZRtp::setMultiStrParams(std::string parameters) {

    char tmp[MAX_DIGEST_LENGTH + 1 + 1 + 1]; // max. hash length + cipher + authLength + hash

    // First get negotiated hash from parameters, set algorithms and length
    int i = parameters.at(0) & 0xff;
    hash = &zrtpHashes.getByOrdinal(i);
    setNegotiatedHash(hash);           // sets hashlength

    // use string.copy(buffer, num, start=0) to retrieve chars (bytes) from the string
    parameters.copy(tmp, hashLength + 1 + 1 + 1, 0);

    i = tmp[1] & 0xff;
    authLength = &zrtpAuthLengths.getByOrdinal(i);
    i = tmp[2] & 0xff;
    cipher = &zrtpSymCiphers.getByOrdinal(i);
    memcpy(zrtpSession, tmp+3, hashLength);

    // after setting zrtpSession, cipher, and auth-length set multi-stream to true
    multiStream = true;
    stateEngine->setMultiStream(true);
}