bool ZrtpStateClass::subEvWaitRelayAck() {
    char *msg, first, last;
    uint8_t* pkt;

    /*
     * First check the general event type, then discrimnate the real event.
     */
    if  (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        last = tolower(*(msg+7));

        /*
         * SAS relayAck:
         * - stop resending SASRelay,
         * - switch to secure substate Normal
         */
        if (first == 'r' && last =='k') {
            cancelTimer();
            secSubstate = Normal;
            sentPacket = NULL;
        }
        return true;
    }
    // Timer event triggered - this is Timer T2 to resend Error.
    else if (event->type == Timer) {
        if (!parent->sendPacketZRTP(sentPacket)) {
            sendFailed(); // returns to state Initial
            return false;
        }
        if (nextTimer(&T2) <= 0) {
            // returns to state initial
            // timerFailed(ZrtpCodes.SevereCodes.SevereTooMuchRetries);
            return false;
        }
        return true;
    }
    return false;
}