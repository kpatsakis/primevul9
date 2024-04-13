static void encoder_init_rle(CommonState *state)
{
    state->melcstate = 0;
    state->melclen = J[0];
    state->melcorder = 1 << state->melclen;
}