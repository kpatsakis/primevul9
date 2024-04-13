AlgorithmEnum* ZRtp::getStrongCipherOffered(ZrtpPacketHello *hello) {

    int num = hello->getNumCiphers();
    for (int i = 0; i < num; i++) {
        if (*(int32_t*)(hello->getCipherType(i)) == *(int32_t*)aes3 ||
            *(int32_t*)(hello->getCipherType(i)) == *(int32_t*)two3) {
            return &zrtpSymCiphers.getByName((const char*)hello->getCipherType(i));
        }
    }
    return NULL;
}