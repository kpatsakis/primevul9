bool ZrtpQueue::isMultiStream()  {
    if (zrtpEngine != NULL)
        return zrtpEngine->isMultiStream();
    return false;
}