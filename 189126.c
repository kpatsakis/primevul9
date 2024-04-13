ZrtpPacketGoClear* ZRtp::prepareGoClear(uint32_t errMsg) {
    ZrtpPacketGoClear* gclr = &zrtpGoClear;
    gclr->clrClearHmac();
    return gclr;
}