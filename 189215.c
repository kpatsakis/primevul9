ZrtpQueue::~ZrtpQueue() {

    endQueue();
    stopZrtp();

    if (zrtpUserCallback != NULL) {
        delete zrtpUserCallback;
        zrtpUserCallback = NULL;
    }
}