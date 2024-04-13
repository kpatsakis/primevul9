ZrtpPacketError* ZRtp::prepareError(uint32_t errMsg) {
    zrtpError.setErrorCode(errMsg);
    return &zrtpError;
}