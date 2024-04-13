int32_t ZRtp::sendPacketZRTP(ZrtpPacketBase *packet) {
    return ((packet == NULL) ? 0 :
            callback->sendDataZRTP(packet->getHeaderBase(), (packet->getLength() * 4) + 4));
}