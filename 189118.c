int32 ZrtpQueue::getSignatureLength() {
    if (zrtpEngine != NULL)
        return zrtpEngine->getSignatureLength();
    return 0;
}