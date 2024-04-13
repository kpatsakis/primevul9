std::string ZrtpQueue::getSasType() {
    if (zrtpEngine != NULL)
        return zrtpEngine->getSasType();
    else
        return NULL;
}