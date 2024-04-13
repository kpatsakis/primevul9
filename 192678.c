S_invlist_iternext(SV* invlist, UV* start, UV* end)
{
    /* An C<invlist_iterinit> call on <invlist> must be used to set this up.
     * This call sets in <*start> and <*end>, the next range in <invlist>.
     * Returns <TRUE> if successful and the next call will return the next
     * range; <FALSE> if was already at the end of the list.  If the latter,
     * <*start> and <*end> are unchanged, and the next call to this function
     * will start over at the beginning of the list */

    STRLEN* pos = get_invlist_iter_addr(invlist);
    UV len = _invlist_len(invlist);
    UV *array;

    PERL_ARGS_ASSERT_INVLIST_ITERNEXT;

    if (*pos >= len) {
	*pos = (STRLEN) UV_MAX;	/* Force iterinit() to be required next time */
	return FALSE;
    }

    array = invlist_array(invlist);

    *start = array[(*pos)++];

    if (*pos >= len) {
	*end = UV_MAX;
    }
    else {
	*end = array[(*pos)++] - 1;
    }

    return TRUE;
}