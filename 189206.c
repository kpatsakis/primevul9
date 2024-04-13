ZrtpPacketClearAck* ZRtp::prepareClearAck(ZrtpPacketGoClear* gpkt) {
    sendInfo(Warning, WarningGoClearReceived);
    return &zrtpClearAck;
}