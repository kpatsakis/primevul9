void ZrtpStateClass::sendFailed() {
    sentPacket = NULL;
    nextState(Initial);
    parent->zrtpNegotiationFailed(Severe, SevereCannotSend);
}