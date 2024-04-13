bool ZrtpQueue::isEnrollmentMode() {
    if (zrtpEngine != NULL)
        return zrtpEngine->isEnrollmentMode();
    else
        return false;
}