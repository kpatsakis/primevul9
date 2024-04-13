Perl__load_PL_utf8_foldclosures (pTHX)
{
    assert(! PL_utf8_foldclosures);

    /* If the folds haven't been read in, call a fold function
     * to force that */
    if (! PL_utf8_tofold) {
        U8 dummy[UTF8_MAXBYTES_CASE+1];
        const U8 hyphen[] = HYPHEN_UTF8;

        /* This string is just a short named one above \xff */
        toFOLD_utf8_safe(hyphen, hyphen + sizeof(hyphen) - 1, dummy, NULL);
        assert(PL_utf8_tofold); /* Verify that worked */
    }
    PL_utf8_foldclosures = _swash_inversion_hash(PL_utf8_tofold);
}