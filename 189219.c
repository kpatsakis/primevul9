void ZrtpStateClass::evCommitSent(void) {

    DEBUGOUT((cout << "Checking for match in CommitSend.\n"));

    char *msg, first, middle, last, secondLast;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        middle = tolower(*(msg+4));
        last = tolower(*(msg+7));
        secondLast = tolower(*(msg+6));

        /*
         * HelloAck or Hello:
         * - delayed "HelloAck" or "Hello", maybe due to network latency, just 
         *   ignore it
         * - no switch in state, leave timer as it is
         */
        if (first == 'h' && middle == 'o' && (last =='k' || last == ' ')) {
            return;
        }

        /*
         * Commit:
         * We have a "Commit" clash. Resolve it.
         *
         * - switch off resending Commit
         * - compare my hvi with peer's hvi
         * - if my hvi is greater
         *   - we are Initiator, stay in state, wait for peer's DHPart1 packet
         * - else
         *   - we are Responder, stop timer
         *   - prepare and send DH1Packt,
         *   - switch to state WaitDHPart2, implies Responder path
         */
        if (first == 'c' && last == ' ') {
            ZrtpPacketCommit zpCo(pkt);

            if (!parent->verifyH2(&zpCo)) {
                return;
            }
            cancelTimer();         // this cancels the Commit timer T2

            // if our hvi is less than peer's hvi: switch to Responder mode and
            // send DHPart1 or Confirm1 packet. Peer (as Initiator) will retrigger if
            // necessary
            //
            if (parent->compareCommit(&zpCo) < 0) {
                if (!multiStream) {
                    ZrtpPacketDHPart* dhPart1 = parent->prepareDHPart1(&zpCo, &errorCode);

                    // Something went wrong during processing of the Commit packet
                    if (dhPart1 == NULL) {
                        if (errorCode != IgnorePacket) {
                            sendErrorPacket(errorCode);
                        }
                        return;
                    }
                    nextState(WaitDHPart2);
                    sentPacket = static_cast<ZrtpPacketBase *>(dhPart1);
                }
                else {
                    ZrtpPacketConfirm* confirm = parent->prepareConfirm1MultiStream(&zpCo, &errorCode);

                    // Something went wrong during processing of the Commit packet
                    if (confirm == NULL) {
                        if (errorCode != IgnorePacket) {
                            sendErrorPacket(errorCode);
                        }
                        return;
                    }
                    nextState(WaitConfirm2);
                    sentPacket = static_cast<ZrtpPacketBase *>(confirm);
                }
                if (!parent->sendPacketZRTP(sentPacket)) {
                    sendFailed();       // returns to state Initial
                }
            }
            // Stay in state, we are Initiator, wait for DHPart1 of Confirm1 packet from peer.
            // Resend Commit after timeout until we get a DHPart1 or Confirm1
            else {
                if (startTimer(&T2) <= 0) { // restart the Commit timer, gives peer more time to react
                    timerFailed(SevereNoTimer);    // returns to state Initial
                }
            }
            return;
        }

        /*
         * DHPart1:
         * - switch off resending Commit
         * - Prepare and send DHPart2
         * - switch to WaitConfirm1
         * - start timer to resend DHPart2 if necessary, we are Initiator
         */
        if (first == 'd' && secondLast == '1') {
            cancelTimer();
            sentPacket = NULL;
            ZrtpPacketDHPart dpkt(pkt);
            ZrtpPacketDHPart* dhPart2 = parent->prepareDHPart2(&dpkt, &errorCode);

            // Something went wrong during processing of the DHPart1 packet
            if (dhPart2 == NULL) {
                if (errorCode != IgnorePacket) {
                    sendErrorPacket(errorCode);
                }
                else {
                    if (startTimer(&T2) <= 0) {
                        timerFailed(SevereNoTimer);       // switches to state Initial
                    }
                }

                return;
            }
            sentPacket = static_cast<ZrtpPacketBase *>(dhPart2);
            nextState(WaitConfirm1);

            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();       // returns to state Initial
                return;
            }
            if (startTimer(&T2) <= 0) {
                timerFailed(SevereNoTimer);       // switches to state Initial
            }
            return;
        }

        /*
         * Confirm1 and multi-stream mode
         * - switch off resending commit
         * - prepare Confirm2
         */
        if (multiStream && (first == 'c' && last == '1')) {
            cancelTimer();
            ZrtpPacketConfirm cpkt(pkt);

            ZrtpPacketConfirm* confirm = parent->prepareConfirm2MultiStream(&cpkt, &errorCode);

            // Something went wrong during processing of the Confirm1 packet
            if (confirm == NULL) {
                sendErrorPacket(errorCode);
                return;
            }
            nextState(WaitConfAck);
            sentPacket = static_cast<ZrtpPacketBase *>(confirm);

            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();         // returns to state Initial
                return;
            }
            if (startTimer(&T2) <= 0) {
                timerFailed(SevereNoTimer);  // returns to state Initial
                return;
            }
            // according to chap 5.6: after sending Confirm2 the Initiator must
            // be ready to receive SRTP data. SRTP sender will be enabled in WaitConfAck
            // state.
            if (!parent->srtpSecretsReady(ForReceiver)) {
                parent->sendInfo(Severe, CriticalSWError);
                sendErrorPacket(CriticalSWError);
                return;
            }
        }
    }
    // Timer event triggered, resend the Commit packet
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();       // returns to state Initial
                return;
        }
        if (nextTimer(&T2) <= 0) {
            timerFailed(SevereTooMuchRetries);       // returns to state Initial
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