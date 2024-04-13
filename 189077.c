void ZRtp::computeSRTPKeys() {

    // allocate the maximum size, compute real size to use
    uint8_t KDFcontext[sizeof(peerZid)+sizeof(ownZid)+sizeof(messageHash)];
    int32_t kdfSize = sizeof(peerZid)+sizeof(ownZid)+hashLength;

    int32_t keyLen = cipher->getKeylen() * 8;

    if (myRole == Responder) {
        memcpy(KDFcontext, peerZid, sizeof(peerZid));
        memcpy(KDFcontext+sizeof(peerZid), ownZid, sizeof(ownZid));
    }
    else {
        memcpy(KDFcontext, ownZid, sizeof(ownZid));
        memcpy(KDFcontext+sizeof(ownZid), peerZid, sizeof(peerZid));
    }
    memcpy(KDFcontext+sizeof(ownZid)+sizeof(peerZid), messageHash, hashLength);

    // Inititiator key and salt
    KDF(s0, hashLength, (unsigned char*)iniMasterKey, strlen(iniMasterKey)+1, KDFcontext, kdfSize, keyLen, srtpKeyI);
    KDF(s0, hashLength, (unsigned char*)iniMasterSalt, strlen(iniMasterSalt)+1, KDFcontext, kdfSize, 112, srtpSaltI);

    // Responder key and salt
    KDF(s0, hashLength, (unsigned char*)respMasterKey, strlen(respMasterKey)+1, KDFcontext, kdfSize, keyLen, srtpKeyR);
    KDF(s0, hashLength, (unsigned char*)respMasterSalt, strlen(respMasterSalt)+1, KDFcontext, kdfSize, 112, srtpSaltR);

    // The HMAC keys for GoClear
    KDF(s0, hashLength, (unsigned char*)iniHmacKey, strlen(iniHmacKey)+1, KDFcontext, kdfSize, hashLength*8, hmacKeyI);
    KDF(s0, hashLength, (unsigned char*)respHmacKey, strlen(respHmacKey)+1, KDFcontext, kdfSize, hashLength*8, hmacKeyR);

    // The keys for Confirm messages
    KDF(s0, hashLength, (unsigned char*)iniZrtpKey, strlen(iniZrtpKey)+1, KDFcontext, kdfSize, keyLen, zrtpKeyI);
    KDF(s0, hashLength, (unsigned char*)respZrtpKey, strlen(respZrtpKey)+1, KDFcontext, kdfSize, keyLen, zrtpKeyR);

    detailInfo.pubKey = detailInfo.sasType = NULL;
    if (!multiStream) {
        // Compute the new Retained Secret
        KDF(s0, hashLength, (unsigned char*)retainedSec, strlen(retainedSec)+1, KDFcontext, kdfSize, SHA256_DIGEST_LENGTH*8, newRs1);

        // Compute the ZRTP Session Key
        KDF(s0, hashLength, (unsigned char*)zrtpSessionKey, strlen(zrtpSessionKey)+1, KDFcontext, kdfSize, hashLength*8, zrtpSession);

        // perform  generation according to chapter 5.5 and 8.
        // we don't need a speciai sasValue filed. sasValue are the first
        // (leftmost) 32 bits (4 bytes) of sasHash
        uint8_t sasBytes[4];
        KDF(s0, hashLength, (unsigned char*)sasString, strlen(sasString)+1, KDFcontext, kdfSize, SHA256_DIGEST_LENGTH*8, sasHash);

        // according to chapter 8 only the leftmost 20 bits of sasValue (aka
        //  sasHash) are used to create the character SAS string of type SAS
        // base 32 (5 bits per character)
        sasBytes[0] = sasHash[0];
        sasBytes[1] = sasHash[1];
        sasBytes[2] = sasHash[2] & 0xf0;
        sasBytes[3] = 0;
        if (*(int32_t*)b32 == *(int32_t*)(sasType->getName())) {
            SAS = Base32(sasBytes, 20).getEncoded();
        }
        else {
            SAS.assign(sas256WordsEven[sasBytes[0]]).append(":").append(sas256WordsOdd[sasBytes[1]]);
        }

        if (signSasSeen)
            callback->signSAS(sasHash);

        detailInfo.pubKey = pubKey->getReadable();
        detailInfo.sasType = sasType->getReadable();
    }
    // set algorithm names into detailInfo structure
    detailInfo.authLength = authLength->getReadable();
    detailInfo.cipher = cipher->getReadable();
    detailInfo.hash = hash->getReadable();

    memset(KDFcontext, 0, sizeof(KDFcontext));
}