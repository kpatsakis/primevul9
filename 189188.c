void ZrtpStateClass::sendSASRelay(ZrtpPacketSASrelay* relay) {
    cancelTimer();
    sentPacket = static_cast<ZrtpPacketBase *>(relay);
    secSubstate = WaitSasRelayAck;
    if (!parent->sendPacketZRTP(static_cast<ZrtpPacketBase *>(relay)) || (startTimer(&T2) <= 0)) {
        sendFailed();
    }
}