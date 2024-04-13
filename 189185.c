std::string ZrtpQueue::getHelloHash(int32_t index)  {
    if (zrtpEngine != NULL)
        return zrtpEngine->getHelloHash(index);
    else
        return std::string();
}