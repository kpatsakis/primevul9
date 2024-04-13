void ZrtpQueue::setMultiStrParams(std::string parameters)  {
    if (zrtpEngine != NULL)
        zrtpEngine->setMultiStrParams(parameters);
}