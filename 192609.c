Perl__setup_canned_invlist(pTHX_ const STRLEN size, const UV element0,
                                 UV** other_elements_ptr)
{
    /* Create and return an inversion list whose contents are to be populated
     * by the caller.  The caller gives the number of elements (in 'size') and
     * the very first element ('element0').  This function will set
     * '*other_elements_ptr' to an array of UVs, where the remaining elements
     * are to be placed.
     *
     * Obviously there is some trust involved that the caller will properly
     * fill in the other elements of the array.
     *
     * (The first element needs to be passed in, as the underlying code does
     * things differently depending on whether it is zero or non-zero) */

    SV* invlist = _new_invlist(size);
    bool offset;

    PERL_ARGS_ASSERT__SETUP_CANNED_INVLIST;

    invlist = add_cp_to_invlist(invlist, element0);
    offset = *get_invlist_offset_addr(invlist);

    invlist_set_len(invlist, size, offset);
    *other_elements_ptr = invlist_array(invlist) + 1;
    return invlist;
}