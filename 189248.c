int32_t ZRtp::compareCommit(ZrtpPacketCommit *commit) {
    // TODO: enhance to compare according to rules defined in chapter 4.2,
    // but we don't support Preshared.
    int32_t len = 0;
    len = !multiStream ? HVI_SIZE : (4 * ZRTP_WORD_SIZE);
    return (memcmp(hvi, commit->getHvi(), len));
}