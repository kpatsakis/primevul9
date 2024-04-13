void ZrtpQueue::signSAS(uint8_t* sasHash) {
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->signSAS(sasHash);
    }
}