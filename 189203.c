AlgorithmEnum* ZRtp::findBestPubkey(ZrtpPacketHello *hello) {

    AlgorithmEnum* peerIntersect[ZrtpConfigure::maxNoOfAlgos+1];
    AlgorithmEnum* ownIntersect[ZrtpConfigure::maxNoOfAlgos+1];

    // Build list of own pubkey algorithm names, must follow the order
    // defined in RFC 6189, chapter 4.1.2.
    const char *orderedAlgos[] = {dh2k, ec25, dh3k, ec38};
    int numOrderedAlgos = sizeof(orderedAlgos) / sizeof(const char*);

    int numAlgosPeer = hello->getNumPubKeys();
    if (numAlgosPeer == 0) {
        hash = &zrtpHashes.getByName(mandatoryHash);             // set mandatory hash
        return &zrtpPubKeys.getByName(mandatoryPubKey);
    }
    // Build own list of intersecting algos, keep own order or algorithms
    // The list must include real public key algorithms only, so skip mult-stream mode, preshared and alike.
    int numAlgosOwn = configureAlgos.getNumConfiguredAlgos(PubKeyAlgorithm);
    int numOwnIntersect = 0;
    for (int i = 0; i < numAlgosOwn; i++) {
        ownIntersect[numOwnIntersect] = &configureAlgos.getAlgoAt(PubKeyAlgorithm, i);
        if (*(int32_t*)(ownIntersect[numOwnIntersect]->getName()) == *(int32_t*)mult) {
            continue;                               // skip multi-stream mode
        }
        for (int ii = 0; ii < numAlgosPeer; ii++) {
            if (*(int32_t*)(ownIntersect[numOwnIntersect]->getName()) == *(int32_t*)(zrtpPubKeys.getByName((const char*)hello->getPubKeyType(ii)).getName())) {
                numOwnIntersect++;
                break;
            }
        }
    }
    // Build list of peer's intersecting algos: take own list as input, order according to sequence in hello packet (peer's order)
    int numPeerIntersect = 0;
    for (int i = 0; i < numAlgosPeer; i++) {
        peerIntersect[numPeerIntersect] = &zrtpPubKeys.getByName((const char*)hello->getPubKeyType(i));
        for (int ii = 0; ii < numOwnIntersect; ii++) {
            if (*(int32_t*)(ownIntersect[ii]->getName()) == *(int32_t*)(peerIntersect[numPeerIntersect]->getName())) {
                numPeerIntersect++;
                break;
            }
        }
    }
    if (numPeerIntersect == 0) {
        // If we don't find a common algorithm - use the mandatory algorithms
        hash = &zrtpHashes.getByName(mandatoryHash);
        return &zrtpPubKeys.getByName(mandatoryPubKey);
    }
    AlgorithmEnum* useAlgo;
    if (numPeerIntersect > 1 && *(int32_t*)(ownIntersect[0]->getName()) != *(int32_t*)(peerIntersect[0]->getName())) {
        int own, peer;

        const int32_t *name = (int32_t*)ownIntersect[0]->getName();
        for (own = 0; own < numOrderedAlgos; own++) {
            if (*name == *(int32_t*)orderedAlgos[own])
                break;
        }
        name = (int32_t*)peerIntersect[0]->getName();
        for (peer = 0; peer < numOrderedAlgos; peer++) {
            if (*name == *(int32_t*)orderedAlgos[peer])
                break;
        }
        if (own < peer) {
            useAlgo = ownIntersect[0];
        }
        else {
            useAlgo = peerIntersect[0];
        }
        // find fastest of conf vs intersecting
    }
    else {
        useAlgo = peerIntersect[0];
    }
    // select a corresponding strong hash if necessary.
    if (*(int32_t*)(useAlgo->getName()) == *(int32_t*)ec38) {
        hash = getStrongHashOffered(hello);
        cipher = getStrongCipherOffered(hello);
    }
    else {
        hash = findBestHash(hello);
    }
    return useAlgo;
}