int32 ZrtpQueue::getPeerZid(uint8* data) {
    if (data == NULL)
        return 0;

    if (zrtpEngine != NULL)
        return zrtpEngine->getPeerZid(data);

    return 0;
}