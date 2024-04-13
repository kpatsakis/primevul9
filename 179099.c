void dump_state(const TIFFSTATE *state){
    TRACE(("State: Location %u size %d eof %d data: %p ifd: %d\n", (uint)state->loc,
           (int)state->size, (uint)state->eof, state->data, state->ifd));
}