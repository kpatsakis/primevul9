Perl__new_invlist(pTHX_ IV initial_size)
{

    /* Return a pointer to a newly constructed inversion list, with enough
     * space to store 'initial_size' elements.  If that number is negative, a
     * system default is used instead */

    SV* new_list;

    if (initial_size < 0) {
	initial_size = 10;
    }

    /* Allocate the initial space */
    new_list = newSV_type(SVt_INVLIST);

    /* First 1 is in case the zero element isn't in the list; second 1 is for
     * trailing NUL */
    SvGROW(new_list, TO_INTERNAL_SIZE(initial_size + 1) + 1);
    invlist_set_len(new_list, 0, 0);

    /* Force iterinit() to be used to get iteration to work */
    *get_invlist_iter_addr(new_list) = (STRLEN) UV_MAX;

    *get_invlist_previous_index_addr(new_list) = 0;

    return new_list;
}