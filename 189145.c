void ZRtp::processTimeout() {
    Event_t ev;

    ev.type = Timer;
    if (stateEngine != NULL) {
        stateEngine->processEvent(&ev);
    }
}