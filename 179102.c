int _tiffMapProc(thandle_t hdata, tdata_t* pbase, toff_t* psize) {
    TIFFSTATE *state = (TIFFSTATE *)hdata;

    TRACE(("_tiffMapProc input size: %u, data: %p\n", (uint)*psize, *pbase));
    dump_state(state);

    *pbase = state->data;
    *psize = state->size;
    TRACE(("_tiffMapProc returning size: %u, data: %p\n", (uint)*psize, *pbase));
    return (1);
}