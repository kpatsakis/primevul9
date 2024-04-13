toff_t _tiffSeekProc(thandle_t hdata, toff_t off, int whence) {
    TIFFSTATE *state = (TIFFSTATE *)hdata;

    TRACE(("_tiffSeekProc: off: %u whence: %d \n", (uint)off, whence));
    dump_state(state);
    switch (whence) {
    case 0:
        state->loc = off;
        break;
    case 1:
        state->loc += off;
        break;
    case 2:
        state->loc = state->eof + off;
        break;
    }
    dump_state(state);
    return state->loc;
}