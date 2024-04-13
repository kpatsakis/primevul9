int32_t ZrtpStateClass::nextTimer(zrtpTimer_t *t) {

    t->time += t->time;
    t->time = (t->time > t->capping)? t->capping : t->time;
    t->counter++;
    if (t->counter > t->maxResend) {
        return -1;
    }
    return parent->activateTimer(t->time);
}