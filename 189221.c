void ZrtpStateClass::sendErrorPacket(uint32_t errorCode) {
    cancelTimer();

    ZrtpPacketError* err = parent->prepareError(errorCode);
    parent->zrtpNegotiationFailed(ZrtpError, errorCode);

    sentPacket =  static_cast<ZrtpPacketBase *>(err);
    nextState(WaitErrorAck);
    if (!parent->sendPacketZRTP(static_cast<ZrtpPacketBase *>(err)) || (startTimer(&T2) <= 0)) {
        sendFailed();
    }
}