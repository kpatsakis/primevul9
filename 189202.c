bool ZrtpQueue::isMultiStreamAvailable()  {
    if (zrtpEngine != NULL)
        return zrtpEngine->isMultiStreamAvailable();
    return false;
}