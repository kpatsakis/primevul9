std::string ZrtpQueue::getPeerHelloHash()  {
    if (zrtpEngine != NULL)
        return zrtpEngine->getPeerHelloHash();
    else
        return std::string();
}