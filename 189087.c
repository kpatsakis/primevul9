int32_t ZrtpStateClass::startTimer(zrtpTimer_t *t) {

    t->time = t->start;
    t->counter = 0;
    return parent->activateTimer(t->time);
}