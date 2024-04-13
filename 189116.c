bool ZRtp::checkMultiStream(ZrtpPacketHello *hello) {

    int  i;
    int num = hello->getNumPubKeys();

    // Multi Stream mode is mandatory, thus if nothing is offered then it is supported :-)
    if (num == 0) {
        return true;
    }
    for (i = 0; i < num; i++) {
        if (*(int32_t*)(hello->getPubKeyType(i)) == *(int32_t*)mult) {
            return true;
        }
    }
    return false;
}