void ZRtp::computePBXSecret() {
    // Construct the KDF context as per ZRTP specification chap 7.3.1:
    // ZIDi || ZIDr
    uint8_t KDFcontext[sizeof(peerZid)+sizeof(ownZid)];
    int32_t kdfSize = sizeof(peerZid)+sizeof(ownZid);

    if (myRole == Responder) {
        memcpy(KDFcontext, peerZid, sizeof(peerZid));
        memcpy(KDFcontext+sizeof(peerZid), ownZid, sizeof(ownZid));
    }
    else {
        memcpy(KDFcontext, ownZid, sizeof(ownZid));
        memcpy(KDFcontext+sizeof(ownZid), peerZid, sizeof(peerZid));
    }

    KDF(zrtpSession, hashLength, (unsigned char*)zrtpTrustedMitm, strlen(zrtpTrustedMitm)+1, KDFcontext,
        kdfSize, SHA256_DIGEST_LENGTH * 8, pbxSecretTmpBuffer);

    pbxSecretTmp = pbxSecretTmpBuffer;  // set pointer to buffer, signal PBX secret was computed
}