Perl__invlist_invert(pTHX_ SV* const invlist)
{
    /* Complement the input inversion list.  This adds a 0 if the list didn't
     * have a zero; removes it otherwise.  As described above, the data
     * structure is set up so that this is very efficient */

    PERL_ARGS_ASSERT__INVLIST_INVERT;

    assert(! invlist_is_iterating(invlist));

    /* The inverse of matching nothing is matching everything */
    if (_invlist_len(invlist) == 0) {
	_append_range_to_invlist(invlist, 0, UV_MAX);
	return;
    }

    *get_invlist_offset_addr(invlist) = ! *get_invlist_offset_addr(invlist);
}