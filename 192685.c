S_invlist_highest(SV* const invlist)
{
    /* Returns the highest code point that matches an inversion list.  This API
     * has an ambiguity, as it returns 0 under either the highest is actually
     * 0, or if the list is empty.  If this distinction matters to you, check
     * for emptiness before calling this function */

    UV len = _invlist_len(invlist);
    UV *array;

    PERL_ARGS_ASSERT_INVLIST_HIGHEST;

    if (len == 0) {
	return 0;
    }

    array = invlist_array(invlist);

    /* The last element in the array in the inversion list always starts a
     * range that goes to infinity.  That range may be for code points that are
     * matched in the inversion list, or it may be for ones that aren't
     * matched.  In the latter case, the highest code point in the set is one
     * less than the beginning of this range; otherwise it is the final element
     * of this range: infinity */
    return (ELEMENT_RANGE_MATCHES_INVLIST(len - 1))
           ? UV_MAX
           : array[len - 1] - 1;
}