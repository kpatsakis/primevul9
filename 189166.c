void ZrtpStateClass::evWaitConfirm1(void) {

    DEBUGOUT((cout << "Checking for match in WaitConfirm1.\n"));

    char *msg, first, last;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));

        /*
         * Confirm1:
         * - Switch off resending DHPart2
         * - prepare a Confirm2 packet
         * - switch to state WaitConfAck
         * - set timer to monitor Confirm2 packet, we are initiator
         */
        if (first == 'c' && last == '1') {
            cancelTimer();
            ZrtpPacketConfirm cpkt(pkt);

            ZrtpPacketConfirm* confirm = parent->prepareConfirm2(&cpkt, &errorCode);

            // Something went wrong during processing of the Confirm1 packet
            if (confirm == NULL) {
                sendErrorPacket(errorCode);
                return;
            }
            // according to chap 5.8: after sending Confirm2 the Initiator must
            // be ready to receive SRTP data. SRTP sender will be enabled in WaitConfAck
            // state.
            if (!parent->srtpSecretsReady(ForReceiver)) {
                parent->sendInfo(Severe, CriticalSWError);
                sendErrorPacket(CriticalSWError);
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
            }
        }
    }
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed();             // returns to state Initial
            return;
        }
        if (nextTimer(&T2) <= 0) {
            timerFailed(SevereTooMuchRetries);     // returns to state Initial
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