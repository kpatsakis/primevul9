bool ZrtpQueue::setSignatureData(uint8* data, int32 length) {
    if (zrtpEngine != NULL)
        return zrtpEngine->setSignatureData(data, length);
    return 0;
}