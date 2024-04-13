void ZrtpQueue::acceptEnrollment(bool accepted) {
    if (zrtpEngine != NULL)
        zrtpEngine->acceptEnrollment(accepted);
}