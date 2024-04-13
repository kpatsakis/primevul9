Perl__invlist_populate_swatch(SV* const invlist,
                              const UV start, const UV end, U8* swatch)
{
    /* populates a swatch of a swash the same way swatch_get() does in utf8.c,
     * but is used when the swash has an inversion list.  This makes this much
     * faster, as it uses a binary search instead of a linear one.  This is
     * intimately tied to that function, and perhaps should be in utf8.c,
     * except it is intimately tied to inversion lists as well.  It assumes
     * that <swatch> is all 0's on input */

    UV current = start;
    const IV len = _invlist_len(invlist);
    IV i;
    const UV * array;

    PERL_ARGS_ASSERT__INVLIST_POPULATE_SWATCH;

    if (len == 0) { /* Empty inversion list */
        return;
    }

    array = invlist_array(invlist);

    /* Find which element it is */
    i = _invlist_search(invlist, start);

    /* We populate from <start> to <end> */
    while (current < end) {
        UV upper;

	/* The inversion list gives the results for every possible code point
	 * after the first one in the list.  Only those ranges whose index is
	 * even are ones that the inversion list matches.  For the odd ones,
	 * and if the initial code point is not in the list, we have to skip
	 * forward to the next element */
        if (i == -1 || ! ELEMENT_RANGE_MATCHES_INVLIST(i)) {
            i++;
            if (i >= len) { /* Finished if beyond the end of the array */
                return;
            }
            current = array[i];
	    if (current >= end) {   /* Finished if beyond the end of what we
				       are populating */
                if (LIKELY(end < UV_MAX)) {
                    return;
                }

                /* We get here when the upper bound is the maximum
                 * representable on the machine, and we are looking for just
                 * that code point.  Have to special case it */
                i = len;
                goto join_end_of_list;
            }
        }
        assert(current >= start);

	/* The current range ends one below the next one, except don't go past
	 * <end> */
        i++;
        upper = (i < len && array[i] < end) ? array[i] : end;

	/* Here we are in a range that matches.  Populate a bit in the 3-bit U8
	 * for each code point in it */
        for (; current < upper; current++) {
            const STRLEN offset = (STRLEN)(current - start);
            swatch[offset >> 3] |= 1 << (offset & 7);
        }

      join_end_of_list:

	/* Quit if at the end of the list */
        if (i >= len) {

	    /* But first, have to deal with the highest possible code point on
	     * the platform.  The previous code assumes that <end> is one
	     * beyond where we want to populate, but that is impossible at the
	     * platform's infinity, so have to handle it specially */
            if (UNLIKELY(end == UV_MAX && ELEMENT_RANGE_MATCHES_INVLIST(len-1)))
	    {
                const STRLEN offset = (STRLEN)(end - start);
                swatch[offset >> 3] |= 1 << (offset & 7);
            }
            return;
        }

	/* Advance to the next range, which will be for code points not in the
	 * inversion list */
        current = array[i];
    }

    return;
}