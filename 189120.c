void ZRtp::generateKeysMultiStream() {

    // allocate the maximum size, compute real size to use
    uint8_t KDFcontext[sizeof(peerZid)+sizeof(ownZid)+sizeof(messageHash)];
    int32_t kdfSize = sizeof(peerZid)+sizeof(ownZid)+hashLength;

    if (myRole == Responder) {
        memcpy(KDFcontext, peerZid, sizeof(peerZid));
        memcpy(KDFcontext+sizeof(peerZid), ownZid, sizeof(ownZid));
    }
    else {
        memcpy(KDFcontext, ownZid, sizeof(ownZid));
        memcpy(KDFcontext+sizeof(ownZid), peerZid, sizeof(peerZid));
    }
    memcpy(KDFcontext+sizeof(ownZid)+sizeof(peerZid), messageHash, hashLength);

    KDF(zrtpSession, hashLength, (unsigned char*)zrtpMsk, strlen(zrtpMsk)+1, KDFcontext, kdfSize, hashLength*8, s0);

    memset(KDFcontext, 0, sizeof(KDFcontext));

    computeSRTPKeys();
}