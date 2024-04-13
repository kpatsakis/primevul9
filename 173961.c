static void encode_state_run(Encoder *encoder, CommonState *state, unsigned int runlen)
{
    int hits = 0;

    while (runlen >= state->melcorder) {
        hits++;
        runlen -= state->melcorder;
        if (state->melcstate < MELCSTATES - 1) {
            state->melclen = J[++state->melcstate];
            state->melcorder = (1L << state->melclen);
        }
    }

    /* send the required number of "hit" bits (one per occurrence
       of a run of length melcorder). This number is never too big:
       after 31 such "hit" bits, each "hit" would represent a run of 32K
       pixels.
    */
    encode_ones(encoder, hits);

    encode(encoder, runlen, state->melclen + 1);

    /* adjust melcoder parameters */
    if (state->melcstate) {
        state->melclen = J[--state->melcstate];
        state->melcorder = (1L << state->melclen);
    }
}