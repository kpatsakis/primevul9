std::string ZrtpQueue::getMultiStrParams()  {
    if (zrtpEngine != NULL)
        return zrtpEngine->getMultiStrParams();
    else
        return std::string();
}