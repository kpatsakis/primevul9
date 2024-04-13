void ZrtpStateClass::evAckDetected(void) {

    DEBUGOUT((cout << "Checking for match in AckDetected.\n"));

    char *msg, first, last;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));

#if 1
        /*
         * Implementation for choice 1)
         * Hello:
         * - Acknowledge peer's Hello, sending HelloACK (F4)
         * - switch to state WaitCommit, wait for peer's Commit
         * - we are going to be in the Responder role
         */

        if (first == 'h' && last ==' ') {
            // Parse Hello packet and build an own Commit packet even if the
            // Commit is not send to the peer. We need to do this to check the
            // Hello packet and prepare the shared secret stuff.
            ZrtpPacketHello hpkt(pkt);
            ZrtpPacketCommit* commit = parent->prepareCommit(&hpkt, &errorCode);

            // Something went wrong during processing of the Hello packet, for
            // example wrong version, duplicate ZID.
            if (commit == NULL) {
                sendErrorPacket(errorCode);
                return;
            }
            ZrtpPacketHelloAck *helloAck = parent->prepareHelloAck();
            nextState(WaitCommit);

            // remember packet for easy resend
            sentPacket = static_cast<ZrtpPacketBase *>(helloAck);
            if (!parent->sendPacketZRTP(static_cast<ZrtpPacketBase *>(helloAck))) {
                sendFailed();
            }
        }
#else
        /*
         * Implementation for choice 2)
         * Hello:
         * - Acknowledge peer's Hello by sending Commit (F5)
         *   instead of HelloAck (F4)
         * - switch to state CommitSent
         * - Initiator role, thus start timer T2 to monitor timeout for Commit
         */

        if (first == 'h' && last == ' ') {
            // Parse peer's packet data into a Hello packet
            ZrtpPacketHello hpkt(pkt);
            ZrtpPacketCommit* commit = parent->prepareCommit(&hpkt, &errorCode);
            // Something went wrong during processing of the Hello packet  
            if (commit == NULL) {
                sendErrorPacket(errorCode);
                return;
            }
            nextState(CommitSent);

            // remember packet for easy resend in case timer triggers
            // Timer trigger received in new state CommitSend
            sentPacket = static_cast<ZrtpPacketBase *>(commit);
            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();
                return;
            }
            if (startTimer(&T2) <= 0) {
                timerFailed(SevereNoTimer);
            }
        }
#endif
    }
    else {  // unknown Event type for this state (covers Error and ZrtpClose)
        if (event->type != ZrtpClose) {
            parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
        }
        nextState(Initial);
    }
}