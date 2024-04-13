void ZRtp::setNegotiatedHash(AlgorithmEnum* hash) {
    switch (zrtpHashes.getOrdinal(*hash)) {
    case 0:
        hashLength = SHA256_DIGEST_LENGTH;
        hashFunction = sha256;
        hashListFunction = sha256;

        hmacFunction = hmac_sha256;
        hmacListFunction = hmac_sha256;

        createHashCtx = createSha256Context;
        closeHashCtx = closeSha256Context;
        hashCtxFunction = sha256Ctx;
        hashCtxListFunction = sha256Ctx;
        break;

    case 1:
        hashLength = SHA384_DIGEST_LENGTH;
        hashFunction = sha384;
        hashListFunction = sha384;

        hmacFunction = hmac_sha384;
        hmacListFunction = hmac_sha384;

        createHashCtx = createSha384Context;
        closeHashCtx = closeSha384Context;
        hashCtxFunction = sha384Ctx;
        hashCtxListFunction = sha384Ctx;
        break;
    }
}