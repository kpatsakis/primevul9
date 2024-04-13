void ZRtp::stopZrtp() {
    Event_t ev;

    if (stateEngine != NULL) {
        ev.type = ZrtpClose;
        stateEngine->processEvent(&ev);
    }
}