void ZrtpQueue::zrtpNotSuppOther() {
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->zrtpNotSuppOther();
    }
}