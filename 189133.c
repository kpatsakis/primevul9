int32_t ZrtpQueue::getCurrentProtocolVersion() {
    if (zrtpEngine != NULL)
        return zrtpEngine->getCurrentProtocolVersion();

    return 0;
}