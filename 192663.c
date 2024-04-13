S__append_range_to_invlist(pTHX_ SV* const invlist,
                                 const UV start, const UV end)
{
   /* Subject to change or removal.  Append the range from 'start' to 'end' at
    * the end of the inversion list.  The range must be above any existing
    * ones. */

    UV* array;
    UV max = invlist_max(invlist);
    UV len = _invlist_len(invlist);
    bool offset;

    PERL_ARGS_ASSERT__APPEND_RANGE_TO_INVLIST;

    if (len == 0) { /* Empty lists must be initialized */
        offset = start != 0;
        array = _invlist_array_init(invlist, ! offset);
    }
    else {
	/* Here, the existing list is non-empty. The current max entry in the
	 * list is generally the first value not in the set, except when the
	 * set extends to the end of permissible values, in which case it is
	 * the first entry in that final set, and so this call is an attempt to
	 * append out-of-order */

	UV final_element = len - 1;
	array = invlist_array(invlist);
	if (   array[final_element] > start
	    || ELEMENT_RANGE_MATCHES_INVLIST(final_element))
	{
	    Perl_croak(aTHX_ "panic: attempting to append to an inversion list, but wasn't at the end of the list, final=%" UVuf ", start=%" UVuf ", match=%c",
		     array[final_element], start,
		     ELEMENT_RANGE_MATCHES_INVLIST(final_element) ? 't' : 'f');
	}

        /* Here, it is a legal append.  If the new range begins 1 above the end
         * of the range below it, it is extending the range below it, so the
         * new first value not in the set is one greater than the newly
         * extended range.  */
        offset = *get_invlist_offset_addr(invlist);
	if (array[final_element] == start) {
	    if (end != UV_MAX) {
		array[final_element] = end + 1;
	    }
	    else {
		/* But if the end is the maximum representable on the machine,
                 * assume that infinity was actually what was meant.  Just let
                 * the range that this would extend to have no end */
		invlist_set_len(invlist, len - 1, offset);
	    }
	    return;
	}
    }

    /* Here the new range doesn't extend any existing set.  Add it */

    len += 2;	/* Includes an element each for the start and end of range */

    /* If wll overflow the existing space, extend, which may cause the array to
     * be moved */
    if (max < len) {
	invlist_extend(invlist, len);

        /* Have to set len here to avoid assert failure in invlist_array() */
        invlist_set_len(invlist, len, offset);

	array = invlist_array(invlist);
    }
    else {
	invlist_set_len(invlist, len, offset);
    }

    /* The next item on the list starts the range, the one after that is
     * one past the new range.  */
    array[len - 2] = start;
    if (end != UV_MAX) {
	array[len - 1] = end + 1;
    }
    else {
	/* But if the end is the maximum representable on the machine, just let
	 * the range have no end */
	invlist_set_len(invlist, len - 1, offset);
    }
}