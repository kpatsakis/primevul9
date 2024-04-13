bool ZRtp::inState(int32_t state)
{
    if (stateEngine != NULL) {
        return stateEngine->inState(state);
    }
    else {
        return false;
    }
}