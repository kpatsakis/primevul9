bool ZrtpQueue::isPeerEnrolled() {
    if (zrtpEngine != NULL)
        return zrtpEngine->isPeerEnrolled();
    else
        return false;
}