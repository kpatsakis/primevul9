const uint8* ZrtpQueue::getSignatureData() {
    if (zrtpEngine != NULL)
        return zrtpEngine->getSignatureData();
    return 0;
}