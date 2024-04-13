int _tiffCloseProc(thandle_t hdata) {
    TIFFSTATE *state = (TIFFSTATE *)hdata;

    TRACE(("_tiffCloseProc \n"));
    dump_state(state);

    return 0;
}