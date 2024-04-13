Perl__invlistEQ(pTHX_ SV* const a, SV* const b, const bool complement_b)
{
    /* Return a boolean as to if the two passed in inversion lists are
     * identical.  The final argument, if TRUE, says to take the complement of
     * the second inversion list before doing the comparison */

    const UV* array_a = invlist_array(a);
    const UV* array_b = invlist_array(b);
    UV len_a = _invlist_len(a);
    UV len_b = _invlist_len(b);

    PERL_ARGS_ASSERT__INVLISTEQ;

    /* If are to compare 'a' with the complement of b, set it
     * up so are looking at b's complement. */
    if (complement_b) {

        /* The complement of nothing is everything, so <a> would have to have
         * just one element, starting at zero (ending at infinity) */
        if (len_b == 0) {
            return (len_a == 1 && array_a[0] == 0);
        }
        else if (array_b[0] == 0) {

            /* Otherwise, to complement, we invert.  Here, the first element is
             * 0, just remove it.  To do this, we just pretend the array starts
             * one later */

            array_b++;
            len_b--;
        }
        else {

            /* But if the first element is not zero, we pretend the list starts
             * at the 0 that is always stored immediately before the array. */
            array_b--;
            len_b++;
        }
    }

    return    len_a == len_b
           && memEQ(array_a, array_b, len_a * sizeof(array_a[0]));

}