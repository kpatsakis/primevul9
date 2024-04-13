static int decode_state_run(Encoder *encoder, CommonState *state)
{
    int runlen = 0;

    do {
        register int temp, hits;
        temp = lzeroes[(BYTE)(~(encoder->io_word >> 24))];/* number of leading ones in the
                                                                      input stream, up to 8 */
        for (hits = 1; hits <= temp; hits++) {
            runlen += state->melcorder;

            if (state->melcstate < MELCSTATES - 1) {
                state->melclen = J[++state->melcstate];
                state->melcorder = (1U << state->melclen);
            }
        }
        if (temp != 8) {
            decode_eatbits(encoder, temp + 1);  /* consume the leading
                                                            0 of the remainder encoding */
            break;
        }
        decode_eatbits(encoder, 8);
    } while (1);

    /* read the length of the remainder */
    if (state->melclen) {
        runlen += encoder->io_word >> (32 - state->melclen);
        decode_eatbits(encoder, state->melclen);
    }

    /* adjust melcorder parameters */
    if (state->melcstate) {
        state->melclen = J[--state->melcstate];
        state->melcorder = (1U << state->melclen);
    }

    return runlen;
}