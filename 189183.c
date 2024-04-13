void ZrtpQueue::startZrtp() {
    if (zrtpEngine != NULL) {
        zrtpEngine->startZrtpEngine();
        zrtpUnprotect = 0;
        started = true;
    }
}