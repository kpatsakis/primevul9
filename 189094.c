std::string ZRtp::getMultiStrParams() {

    // the string will hold binary data - it's opaque to the application
    std::string str("");
    char tmp[MAX_DIGEST_LENGTH + 1 + 1 + 1]; // hash length + cipher + authLength + hash

    if (inState(SecureState) && !multiStream) {
        // construct array that holds zrtpSession, cipher type, auth-length, and hash type
        tmp[0] = zrtpHashes.getOrdinal(*hash);
        tmp[1] = zrtpAuthLengths.getOrdinal(*authLength);
        tmp[2] = zrtpSymCiphers.getOrdinal(*cipher);
        memcpy(tmp+3, zrtpSession, hashLength);
        str.assign(tmp, hashLength + 1 + 1 + 1); // set chars (bytes) to the string
    }
    return str;
}