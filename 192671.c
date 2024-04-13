Perl__new_invlist_C_array(pTHX_ const UV* const list)
{
    /* Return a pointer to a newly constructed inversion list, initialized to
     * point to <list>, which has to be in the exact correct inversion list
     * form, including internal fields.  Thus this is a dangerous routine that
     * should not be used in the wrong hands.  The passed in 'list' contains
     * several header fields at the beginning that are not part of the
     * inversion list body proper */

    const STRLEN length = (STRLEN) list[0];
    const UV version_id =          list[1];
    const bool offset   =    cBOOL(list[2]);
#define HEADER_LENGTH 3
    /* If any of the above changes in any way, you must change HEADER_LENGTH
     * (if appropriate) and regenerate INVLIST_VERSION_ID by running
     *      perl -E 'say int(rand 2**31-1)'
     */
#define INVLIST_VERSION_ID 148565664 /* This is a combination of a version and
                                        data structure type, so that one being
                                        passed in can be validated to be an
                                        inversion list of the correct vintage.
                                       */

    SV* invlist = newSV_type(SVt_INVLIST);

    PERL_ARGS_ASSERT__NEW_INVLIST_C_ARRAY;

    if (version_id != INVLIST_VERSION_ID) {
        Perl_croak(aTHX_ "panic: Incorrect version for previously generated inversion list");
    }

    /* The generated array passed in includes header elements that aren't part
     * of the list proper, so start it just after them */
    SvPV_set(invlist, (char *) (list + HEADER_LENGTH));

    SvLEN_set(invlist, 0);  /* Means we own the contents, and the system
			       shouldn't touch it */

    *(get_invlist_offset_addr(invlist)) = offset;

    /* The 'length' passed to us is the physical number of elements in the
     * inversion list.  But if there is an offset the logical number is one
     * less than that */
    invlist_set_len(invlist, length  - offset, offset);

    invlist_set_previous_index(invlist, 0);

    /* Initialize the iteration pointer. */
    invlist_iterfinish(invlist);

    SvREADONLY_on(invlist);

    return invlist;
}