void ZrtpQueue::zrtpInformEnrollment(GnuZrtpCodes::InfoEnrollment  info) {
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->zrtpInformEnrollment(info);
    }
}