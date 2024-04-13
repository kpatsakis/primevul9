void ZrtpQueue::setAuxSecret(uint8* data, int32_t length)  {
    if (zrtpEngine != NULL)
        zrtpEngine->setAuxSecret(data, length);
}