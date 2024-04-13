uint8_t* ZrtpQueue::getSasHash() {
    if (zrtpEngine != NULL)
        return zrtpEngine->getSasHash();
    else
        return NULL;
}