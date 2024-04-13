void ZRtp::conf2AckSecure() {
    Event_t ev;

    ev.type = ZrtpPacket;
    ev.packet = (uint8_t*)zrtpConf2Ack.getHeaderBase();
    ev.length = sizeof (Conf2AckPacket_t) + 12;  // 12 is fixed ZRTP (RTP) header size

    if (stateEngine != NULL) {
        stateEngine->processEvent(&ev);
    }
}