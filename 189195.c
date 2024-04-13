void ZrtpStateClass::evWaitConfirm2(void) {

    DEBUGOUT((cout << "Checking for match in WaitConfirm2.\n"));

    char *msg, first, secondLast, last;
    uint8_t *pkt;
    uint32_t errorCode = 0;

    if (event->type == ZrtpPacket) {
        pkt = event->packet;
        msg = (char *)pkt + 4;

        first = tolower(*msg);
        secondLast = tolower(*(msg+6));
        last = tolower(*(msg+7));

        /*
         * DHPart2 or Commit in multi stream mode:
         * - resend Confirm1 packet
         * - stay in state
         */
        if ((first == 'd' && secondLast == '2') || (multiStream && (first == 'c' && last == ' '))) {
            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();             // returns to state Initial
            }
            return;
        }
        /*
         * Confirm2:
         * - prepare ConfAck
         * - switch on security (SRTP)
         * - switch to SecureState
         */
        if (first == 'c' && last == '2') {
            ZrtpPacketConfirm cpkt(pkt);
            ZrtpPacketConf2Ack* confack = parent->prepareConf2Ack(&cpkt, &errorCode);

            // Something went wrong during processing of the confirm2 packet
            if (confack == NULL) {
                sendErrorPacket(errorCode);
                return;
            }
            sentPacket = static_cast<ZrtpPacketBase *>(confack);

            if (!parent->sendPacketZRTP(sentPacket)) {
                sendFailed();             // returns to state Initial
                return;
            }
            if (!parent->srtpSecretsReady(ForReceiver) || !parent->srtpSecretsReady(ForSender))  {
                parent->sendInfo(Severe, CriticalSWError);
                sendErrorPacket(CriticalSWError);
                return;
            }
            nextState(SecureState);
            parent->sendInfo(Info, InfoSecureStateOn);
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