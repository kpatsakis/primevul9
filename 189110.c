void ZrtpStateClass::evDetect(void) {

    DEBUGOUT((cout << "Checking for match in Detect.\n"));

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
         * - our peer acknowledged our Hello packet, we have not seen the peer's Hello yet
         * - cancel timer T1 to stop resending Hello
         * - switch to state AckDetected, wait for peer's Hello (F3)
         * 
         * When we receive an HelloAck this also means that out partner accepted our protocol version.
         */
        if (first == 'h' && last =='k') {
            cancelTimer();
            sentPacket = NULL;
            nextState(AckDetected);
            return;
        }
        /*
         * Hello:
         * - send HelloAck packet to acknowledge the received Hello packet if versions match.
         *   Otherweise negotiate ZRTP versions.
         * - use received Hello packet to prepare own Commit packet. We need to
         *   do it at this point because we need the hash value computed from
         *   peer's Hello packet. Follwing states my use the prepared Commit.
         * - switch to new state AckSent which sends own Hello packet until 
         *   peer acknowledges this
         * - Don't clear sentPacket, points to Hello
         */
        if (first == 'h' && last ==' ') {
            ZrtpPacketHello hpkt(pkt);

            cancelTimer();

            /*
             * Check and negotiate the ZRTP protocol version first.
             *
             * This selection mechanism relies on the fact that we sent the highest supported protocol version in
             * the initial Hello packet with as stated in RFC6189, section 4.1.1
             */
            int32_t recvVersion = hpkt.getVersionInt();
            if (recvVersion > sentVersion) {   // We don't support this version, stay in state with timer active
                if (startTimer(&T1) <= 0) {
                    timerFailed(SevereNoTimer);      // returns to state Initial
                }
                return;
            }

            /*
             * The versions don't match. Start negotiating versions. This negotiation stays in the Detect state.
             * Only if the received version matches our own sent version we start to send a HelloAck.
             */
            if (recvVersion != sentVersion) {
                ZRtp::HelloPacketVersion* hpv = parent->helloPackets;

                int32_t index;
                for (index = 0; hpv->packet && hpv->packet != parent->currentHelloPacket; hpv++, index++)   // Find current sent Hello
                    ;

                for(; index >= 0 && hpv->version > recvVersion; hpv--, index--)   // find a supported version less-equal to received version
                    ;

                if (index < 0) {
                    sendErrorPacket(UnsuppZRTPVersion);
                    return;
                }
                parent->currentHelloPacket = hpv->packet;
                sentVersion = parent->currentHelloPacket->getVersionInt();

                // remember packet for easy resend in case timer triggers
                sentPacket = static_cast<ZrtpPacketBase *>(parent->currentHelloPacket);

                if (!parent->sendPacketZRTP(sentPacket)) {
                    sendFailed();                 // returns to state Initial
                    return;
                }
                if (startTimer(&T1) <= 0) {
                    timerFailed(SevereNoTimer);      // returns to state Initial
                    return;
                }
                return;
            }
            ZrtpPacketHelloAck* helloAck = parent->prepareHelloAck();

            if (!parent->sendPacketZRTP(static_cast<ZrtpPacketBase *>(helloAck))) {
                parent->zrtpNegotiationFailed(Severe, SevereCannotSend);
                return;
            }
            // Use peer's Hello packet to create my commit packet, store it 
            // for possible later usage in state AckSent
            commitPkt = parent->prepareCommit(&hpkt, &errorCode);

            nextState(AckSent);
            if (commitPkt == NULL) {
                sendErrorPacket(errorCode);    // switches to Error state
                return;
            }
            if (startTimer(&T1) <= 0) {        // restart own Hello timer/counter
                timerFailed(SevereNoTimer);    // returns to state Initial
            }
            T1.maxResend = 60;                 // more retries to extend time, see chap. 6
        }
        return;      // unknown packet for this state - Just ignore it
    }
    // Timer event triggered - this is Timer T1 to resend Hello
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed();       // returns to state Initial
            return;
        }
        if (nextTimer(&T1) <= 0) {
            commitPkt = NULL;
            parent->zrtpNotSuppOther();
            nextState(Detect);
        }
    }
    // If application calls zrtpStart() to restart discovery
    else if (event->type == ZrtpInitial) {
        cancelTimer();
        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed();                 // returns to state Initial
            return;
        }
        if (startTimer(&T1) <= 0) {
            timerFailed(SevereNoTimer);   // returns to state Initial
        }
    }
    else { // unknown Event type for this state (covers Error and ZrtpClose)
        if (event->type != ZrtpClose) {
            parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
        }
        sentPacket = NULL;
        nextState(Initial);
    }
}