AlgorithmEnum* ZRtp::getStrongHashOffered(ZrtpPacketHello *hello) {

    int numHash = hello->getNumHashes();
    for (int i = 0; i < numHash; i++) {
        if (*(int32_t*)(hello->getHashType(i)) == *(int32_t*)s384) {
            return &zrtpHashes.getByName((const char*)hello->getHashType(i));
        }
    }
    return NULL;
}