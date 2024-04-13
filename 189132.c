void ZrtpQueue::zrtpAskEnrollment(GnuZrtpCodes::InfoEnrollment  info) {
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->zrtpAskEnrollment(info);
    }
}