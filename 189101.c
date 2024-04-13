void ZrtpStateClass::evWaitCommit(void) {

    DEBUGOUT((cout << "Checking for match in WaitCommit.\n"));

    char *msg, first, last;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));
        /*
         * Hello:
         * - resend HelloAck
         * - stay in WaitCommit
         */
        if (first == 'h' && last == ' ') {
            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();       // returns to state Initial
            }
            return;
        }
        /*
         * Commit:
         * - prepare DH1Part packet or Confirm1 if multi stream mode
         * - send it to peer
         * - switch state to WaitDHPart2 or WaitConfirm2 if multi stream mode
         * - don't start timer, we are responder
         */
        if (first == 'c' && last == ' ') {
            ZrtpPacketCommit cpkt(pkt);

            if (!multiStream) {
                ZrtpPacketDHPart* dhPart1 = parent->prepareDHPart1(&cpkt, &errorCode);

                // Something went wrong during processing of the Commit packet
                if (dhPart1 == NULL) {
                    if (errorCode != IgnorePacket) {
                        sendErrorPacket(errorCode);
                    }
                    return;
                }
                sentPacket = static_cast<ZrtpPacketBase *>(dhPart1);
                nextState(WaitDHPart2);
            }
            else {
                ZrtpPacketConfirm* confirm = parent->prepareConfirm1MultiStream(&cpkt, &errorCode);

                // Something went wrong during processing of the Commit packet
                if (confirm == NULL) {
                    if (errorCode != IgnorePacket) {
                        sendErrorPacket(errorCode);
                    }
                    return;
                }
                sentPacket = static_cast<ZrtpPacketBase *>(confirm);
                nextState(WaitConfirm2);
            }
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