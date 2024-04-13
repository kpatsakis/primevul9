S_compute_EXACTish(RExC_state_t *pRExC_state)
{
    U8 op;

    PERL_ARGS_ASSERT_COMPUTE_EXACTISH;

    if (! FOLD) {
        return (LOC)
                ? EXACTL
                : EXACT;
    }

    op = get_regex_charset(RExC_flags);
    if (op >= REGEX_ASCII_RESTRICTED_CHARSET) {
        op--; /* /a is same as /u, and map /aa's offset to what /a's would have
                 been, so there is no hole */
    }

    return op + EXACTF;
}