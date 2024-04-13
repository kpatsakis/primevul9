toff_t _tiffSizeProc(thandle_t hdata) {
    TIFFSTATE *state = (TIFFSTATE *)hdata;

    TRACE(("_tiffSizeProc \n"));
    dump_state(state);

    return (toff_t)state->size;
}