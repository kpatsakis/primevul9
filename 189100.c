ZrtpStateClass::~ZrtpStateClass(void) {

    // If not in Initial state: close the protocol engine
    // before destroying it. This will free pending packets
    // if necessary.
    if (!inState(Initial)) {
        Event_t ev;

        cancelTimer();
        ev.type = ZrtpClose;
        event = &ev;
        engine->processEvent(*this);
    }
    delete engine;
}