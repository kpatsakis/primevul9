bool ZRtp::handleGoClear(uint8_t *message)
{
    char *msg, first, last;

    msg = (char *)message + 4;
    first = tolower(*msg);
    last = tolower(*(msg+6));

    if (first == 'g' && last == 'r') {
        Event_t ev;

        ev.type = ZrtpGoClear;
        ev.packet = message;
        if (stateEngine != NULL) {
            stateEngine->processEvent(&ev);
        }
        return true;
    }
    else {
        return false;
    }
}