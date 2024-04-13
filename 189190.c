ZrtpPacketErrorAck* ZRtp::prepareErrorAck(ZrtpPacketError* epkt) {
    sendInfo(ZrtpError, epkt->getErrorCode() * -1);
    return &zrtpErrorAck;
}