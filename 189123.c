int32_t ZrtpQueue::sendDataZRTP(const unsigned char *data, int32_t length) {

    OutgoingZRTPPkt* packet = new OutgoingZRTPPkt(data, length);

    packet->setSSRC(getLocalSSRC());

    packet->setSeqNum(senderZrtpSeqNo++);

    /*
     * Compute the ZRTP CRC over the full ZRTP packet. Thus include
     * the fixed packet header into the calculation.
     */
    uint16_t temp = packet->getRawPacketSize() - CRC_SIZE;
    uint8_t* pt = (uint8_t*)packet->getRawPacket();
    uint32_t crc = zrtpGenerateCksum(pt, temp);
    // convert and store CRC in crc field of ZRTP packet.
    crc = zrtpEndCksum(crc);

    // advance pointer to CRC storage
    pt += temp;
    *(uint32_t*)pt = htonl(crc);

    dispatchImmediate(packet);
    delete packet;

    return 1;
}