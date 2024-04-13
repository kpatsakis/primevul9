int32_t ZRtp::getPeerZid(uint8_t* data) {
    memcpy(data, peerZid, IDENTIFIER_LEN);
    return IDENTIFIER_LEN;
}