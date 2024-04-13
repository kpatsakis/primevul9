void ZrtpStateClass::evWaitErrorAck(void) {
    DEBUGOUT((cout << "Checking for match in ErrorAck.\n"));

    char *msg, first, last;
    uint8_t *pkt;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));

        /*
         * Errorck:
         * - stop resending Error,
         * - switch to state Initial
         */
        if (first == 'e' && last =='k') {
            cancelTimer();
            sentPacket = NULL;
            nextState(Initial);
        }
    }
    // Timer event triggered - this is Timer T2 to resend Error.
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed();                 // returns to state Initial
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