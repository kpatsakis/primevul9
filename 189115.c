void ZrtpStateClass::evSecureState(void) {

    DEBUGOUT((cout << "Checking for match in SecureState.\n"));

    char *msg, first, last;
    uint8_t *pkt;

    /*
     * Handle a possible substate. If substate handling was ok just return.
     */
    if (secSubstate == WaitSasRelayAck) {
        if (subEvWaitRelayAck())
            return; 
    }

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));

        /*
         * Confirm2:
         * - resend Conf2Ack packet
         * - stay in state
         */
        if (first == 'c' && last == '2') {
            if (sentPacket != NULL && !parent->sendPacketZRTP(sentPacket)) {
                sentPacket = NULL;
                nextState(Initial);
                parent->srtpSecretsOff(ForSender);
                parent->srtpSecretsOff(ForReceiver);
                parent->zrtpNegotiationFailed(Severe, SevereCannotSend);
            }
            return;
        }
        /*
         * GoClear received, handle it. TODO fix go clear handling
         *
        if (first == 'g' && last == 'r') {
            ZrtpPacketGoClear gpkt(pkt);
            ZrtpPacketClearAck* clearAck = parent->prepareClearAck(&gpkt);

            if (!parent->sendPacketZRTP(static_cast<ZrtpPacketBase *>(clearAck))) {
                return;
            }
        // TODO Timeout to resend clear ack until user user confirmation
        }
        */
    }
    else if (event->type == Timer) {
        // Ignore stray timeout in this state
        ;
    }
    // unknown Event type for this state (covers Error and ZrtpClose)
    else  {
        // If in secure state ingnore error events to avoid Error packet injection
        // attack - found by Dmitry Monakhov (dmonakhov@openvz.org)
        if (event->type == ErrorPkt)
            return;
        sentPacket = NULL;
        parent->srtpSecretsOff(ForSender);
        parent->srtpSecretsOff(ForReceiver);
        nextState(Initial);
        if (event->type != ZrtpClose) {
            parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
        }
        parent->sendInfo(Info, InfoSecureStateOff);
    }
}