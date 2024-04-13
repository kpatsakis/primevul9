int32_t ZrtpQueue::getNumberSupportedVersions() {
    if (zrtpEngine != NULL)
        return zrtpEngine->getNumberSupportedVersions();

    return 0;
}