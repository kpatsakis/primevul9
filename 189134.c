void ZRtp::processZrtpMessage(uint8_t *message, uint32_t pSSRC, size_t length) {
    Event_t ev;

    peerSSRC = pSSRC;
    ev.type = ZrtpPacket;
    ev.length = length;
    ev.packet = message;

    if (stateEngine != NULL) {
        stateEngine->processEvent(&ev);
    }
}