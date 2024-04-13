void ZrtpStateClass::timerFailed(int32_t subCode) {
    sentPacket = NULL;
    nextState(Initial);
    parent->zrtpNegotiationFailed(Severe, subCode);
}