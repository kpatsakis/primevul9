void ZrtpStateClass::evWaitDHPart2(void) {

    DEBUGOUT((cout << "Checking for match in DHPart2.\n"));

    char *msg, first, secondLast, last;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));
        secondLast = tolower(*(msg+6));
        /*
         * Commit:
         * - resend DHPart1
         * - stay in state
         */
        if (first == 'c' && last == ' ') {
            if (!parent->sendPacketZRTP(sentPacket)) {
                return sendFailed();       // returns to state Initial
            }
            return;
        }
        /*
         * DHPart2:
         * - prepare Confirm1 packet
         * - switch to WaitConfirm2
         * - No timer, we are responder
         */
        if (first == 'd' && secondLast == '2') {
            ZrtpPacketDHPart dpkt(pkt);
            ZrtpPacketConfirm* confirm = parent->prepareConfirm1(&dpkt, &errorCode);

            if (confirm == NULL) {
                if (errorCode != IgnorePacket) {
                    sendErrorPacket(errorCode);
                }
                return;
            }
            nextState(WaitConfirm2);
            sentPacket = static_cast<ZrtpPacketBase *>(confirm);
            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();       // returns to state Initial
            }
        }
    }
    else {  // unknown Event type for this state (covers Error and ZrtpClose)
        if (event->type != ZrtpClose) {
            parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
        }
        sentPacket = NULL;
        nextState(Initial);
    }
}