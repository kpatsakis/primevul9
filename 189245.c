void ZrtpStateClass::evWaitConfAck(void) {

    DEBUGOUT((cout << "Checking for match in WaitConfAck.\n"));

    char *msg, first, last;
    uint8_t *pkt;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));
         /*
         * ConfAck:
         * - Switch off resending Confirm2
         * - switch to SecureState
         */
        if (first == 'c' && last == 'k') {
            cancelTimer();
            sentPacket = NULL;
            // Receiver was already enabled after sending Confirm2 packet
            // see previous states.
            if (!parent->srtpSecretsReady(ForSender)) {
                parent->sendInfo(Severe, CriticalSWError);
                sendErrorPacket(CriticalSWError);
                return;
            }
            nextState(SecureState);
            // TODO: call parent to clear signature data at initiator
            parent->sendInfo(Info, InfoSecureStateOn);
        }
    }
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed();             // returns to state Initial
            parent->srtpSecretsOff(ForReceiver);
            return;
        }
        if (nextTimer(&T2) <= 0) {
            timerFailed(SevereTooMuchRetries); // returns to state Initial
            parent->srtpSecretsOff(ForReceiver);
        }
    }
    else {  // unknown Event type for this state (covers Error and ZrtpClose)
        if (event->type != ZrtpClose) {
            parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
        }
        sentPacket = NULL;
        nextState(Initial);
        parent->srtpSecretsOff(ForReceiver);
    }
}