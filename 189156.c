void ZrtpStateClass::evAckSent(void) {

    DEBUGOUT((cout << "Checking for match in AckSent.\n"));

    char *msg, first, last;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    /*
     * First check the general event type, then discrimnate
     * the real event.
     */
    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));

        /*
         * HelloAck:
         * The peer answers with HelloAck to own HelloAck/Hello. Send Commit
         * and try Initiator mode. The requirement defined in chapter 4.1 to
         * have a complete Hello/HelloAck is fulfilled.
         * - stop Hello timer T1
         * - send own Commit message
         * - switch state to CommitSent, start Commit timer, assume Initiator
         */
        if (first == 'h' && last =='k') {
            cancelTimer();

            // remember packet for easy resend in case timer triggers
            // Timer trigger received in new state CommitSend
            sentPacket = static_cast<ZrtpPacketBase *>(commitPkt);
            commitPkt = NULL;                    // now stored in sentPacket
            nextState(CommitSent);
            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();             // returns to state Initial
                return;
            }
            if (startTimer(&T2) <= 0) {
                timerFailed(SevereNoTimer);  // returns to state Initial
            }
            return;
        }
        /*
         * Hello:
         * - peer didn't receive our HelloAck
         * - repeat HelloAck/Hello response:
         *  -- get HelloAck packet, send it
         *  -- The timeout trigger of T1 sends our Hello packet
         *  -- stay in state AckSent
         *
         * Similar to Detect state: just acknowledge the Hello, the next
         * timeout sends the following Hello.
         */

        if (first == 'h' && last ==' ') {
            ZrtpPacketHelloAck* helloAck = parent->prepareHelloAck();

            if (!parent->sendPacketZRTP(static_cast<ZrtpPacketBase *>(helloAck))) {
                nextState(Detect);
                parent->zrtpNegotiationFailed(Severe, SevereCannotSend);
            }
            return;
        }
        /*
         * Commit:
         * The peer answers with Commit to HelloAck/Hello, thus switch to
         * responder mode.
         * - stop timer T1
         * - prepare and send our DHPart1
         * - switch to state WaitDHPart2 and wait for peer's DHPart2
         * - don't start timer, we are responder
         */
        if (first == 'c' && last == ' ') {
            cancelTimer();
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
                commitPkt = NULL;
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
                sendFailed();      // returns to state Initial
            }
        }
    }
    /*
     * Timer:
     * - resend Hello packet, stay in state, restart timer until repeat
     *   counter triggers
     * - if repeat counter triggers switch to state Detect, con't clear
     *   sentPacket, Detect requires it to point to own Hello message
     */
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
            return sendFailed();      // returns to state Initial
        }
        if (nextTimer(&T1) <= 0) {
            parent->zrtpNotSuppOther();
            commitPkt = NULL;
            // Stay in state Detect to be prepared get an hello from
            // other peer any time later
            nextState(Detect);
        }
    }
    else {   // unknown Event type for this state (covers Error and ZrtpClose)
        if (event->type != ZrtpClose) {
            parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
        }
        commitPkt = NULL;
        sentPacket = NULL;
        nextState(Initial);
    }
}