void ZrtpStateClass::evInitial(void) {
    DEBUGOUT((cout << "Checking for match in Initial.\n"));

    if (event->type == ZrtpInitial) {
        ZrtpPacketHello* hello = parent->prepareHello();
        sentVersion = hello->getVersionInt();

        // remember packet for easy resend in case timer triggers
        sentPacket = static_cast<ZrtpPacketBase *>(hello);

        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed();                 // returns to state Initial
            return;
        }
        if (startTimer(&T1) <= 0) {
            timerFailed(SevereNoTimer);      // returns to state Initial
            return;
        }
        nextState(Detect);
    }
}