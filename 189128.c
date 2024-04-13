void ZrtpQueue::stopZrtp() {
    if (zrtpEngine != NULL) {
        if (zrtpUnprotect < 50 && !zrtpEngine->isMultiStream())
            zrtpEngine->setRs2Valid();
        delete zrtpEngine;
        zrtpEngine = NULL;
        started = false;
    }
}