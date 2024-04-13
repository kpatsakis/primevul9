void ZrtpQueue::handleTimeout(const std::string &c) {
    if (zrtpEngine != NULL) {
        zrtpEngine->processTimeout();
    }
}