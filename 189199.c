void ZRtp::startZrtpEngine() {
    Event_t ev;

    if (stateEngine != NULL && stateEngine->inState(Initial)) {
        ev.type = ZrtpInitial;
        stateEngine->processEvent(&ev);
    }
}