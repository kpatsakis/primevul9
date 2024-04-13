ZrtpPacketHello::ZrtpPacketHello(uint8_t *data) {
    DEBUGOUT((fprintf(stdout, "Creating Hello packet from data\n")));

    zrtpHeader = (zrtpPacketHeader_t *)&((HelloPacket_t *)data)->hdr;	// the standard header
    helloHeader = (Hello_t *)&((HelloPacket_t *)data)->hello;

    uint32_t t = *((uint32_t*)&helloHeader->flags);
    uint32_t temp = zrtpNtohl(t);

    nHash = (temp & (0xf << 16)) >> 16;
    nHash &= 0x7;                              // restrict to max 7 algorithms
    nCipher = (temp & (0xf << 12)) >> 12;
    nCipher &= 0x7;
    nAuth = (temp & (0xf << 8)) >> 8;
    nAuth &= 0x7;
    nPubkey = (temp & (0xf << 4)) >> 4;
    nPubkey &= 0x7;
    nSas = temp & 0xf;
    nSas &= 0x7;

    // +2 : the MAC at the end of the packet
    computedLength = nHash + nCipher + nAuth + nPubkey + nSas + sizeof(HelloPacket_t)/ZRTP_WORD_SIZE + 2;

    oHash = sizeof(Hello_t);
    oCipher = oHash + (nHash * ZRTP_WORD_SIZE);
    oAuth = oCipher + (nCipher * ZRTP_WORD_SIZE);
    oPubkey = oAuth + (nAuth * ZRTP_WORD_SIZE);
    oSas = oPubkey + (nPubkey * ZRTP_WORD_SIZE);
    oHmac = oSas + (nSas * ZRTP_WORD_SIZE);         // offset to HMAC
}