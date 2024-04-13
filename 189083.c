void ZrtpQueue::setEnrollmentMode(bool enrollmentMode) {
    if (zrtpEngine != NULL)
        zrtpEngine->setEnrollmentMode(enrollmentMode);
}