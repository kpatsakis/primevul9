Perl__invlist_union_maybe_complement_2nd(pTHX_ SV* const a, SV* const b,
                                         const bool complement_b, SV** output)
{
    /* Take the union of two inversion lists and point '*output' to it.  On
     * input, '*output' MUST POINT TO NULL OR TO AN SV* INVERSION LIST (possibly
     * even 'a' or 'b').  If to an inversion list, the contents of the original
     * list will be replaced by the union.  The first list, 'a', may be
     * NULL, in which case a copy of the second list is placed in '*output'.
     * If 'complement_b' is TRUE, the union is taken of the complement
     * (inversion) of 'b' instead of b itself.
     *
     * The basis for this comes from "Unicode Demystified" Chapter 13 by
     * Richard Gillam, published by Addison-Wesley, and explained at some
     * length there.  The preface says to incorporate its examples into your
     * code at your own risk.
     *
     * The algorithm is like a merge sort. */

    const UV* array_a;    /* a's array */
    const UV* array_b;
    UV len_a;	    /* length of a's array */
    UV len_b;

    SV* u;			/* the resulting union */
    UV* array_u;
    UV len_u = 0;

    UV i_a = 0;		    /* current index into a's array */
    UV i_b = 0;
    UV i_u = 0;

    /* running count, as explained in the algorithm source book; items are
     * stopped accumulating and are output when the count changes to/from 0.
     * The count is incremented when we start a range that's in an input's set,
     * and decremented when we start a range that's not in a set.  So this
     * variable can be 0, 1, or 2.  When it is 0 neither input is in their set,
     * and hence nothing goes into the union; 1, just one of the inputs is in
     * its set (and its current range gets added to the union); and 2 when both
     * inputs are in their sets.  */
    UV count = 0;

    PERL_ARGS_ASSERT__INVLIST_UNION_MAYBE_COMPLEMENT_2ND;
    assert(a != b);
    assert(*output == NULL || SvTYPE(*output) == SVt_INVLIST);

    len_b = _invlist_len(b);
    if (len_b == 0) {

        /* Here, 'b' is empty, hence it's complement is all possible code
         * points.  So if the union includes the complement of 'b', it includes
         * everything, and we need not even look at 'a'.  It's easiest to
         * create a new inversion list that matches everything.  */
        if (complement_b) {
            SV* everything = _add_range_to_invlist(NULL, 0, UV_MAX);

            if (*output == NULL) { /* If the output didn't exist, just point it
                                      at the new list */
                *output = everything;
            }
            else { /* Otherwise, replace its contents with the new list */
                invlist_replace_list_destroys_src(*output, everything);
                SvREFCNT_dec_NN(everything);
            }

            return;
        }

        /* Here, we don't want the complement of 'b', and since 'b' is empty,
         * the union will come entirely from 'a'.  If 'a' is NULL or empty, the
         * output will be empty */

        if (a == NULL || _invlist_len(a) == 0) {
            if (*output == NULL) {
                *output = _new_invlist(0);
            }
            else {
                invlist_clear(*output);
            }
            return;
        }

        /* Here, 'a' is not empty, but 'b' is, so 'a' entirely determines the
         * union.  We can just return a copy of 'a' if '*output' doesn't point
         * to an existing list */
        if (*output == NULL) {
            *output = invlist_clone(a);
            return;
        }

        /* If the output is to overwrite 'a', we have a no-op, as it's
         * already in 'a' */
        if (*output == a) {
            return;
        }

        /* Here, '*output' is to be overwritten by 'a' */
        u = invlist_clone(a);
        invlist_replace_list_destroys_src(*output, u);
        SvREFCNT_dec_NN(u);

        return;
    }

    /* Here 'b' is not empty.  See about 'a' */

    if (a == NULL || ((len_a = _invlist_len(a)) == 0)) {

        /* Here, 'a' is empty (and b is not).  That means the union will come
         * entirely from 'b'.  If '*output' is NULL, we can directly return a
         * clone of 'b'.  Otherwise, we replace the contents of '*output' with
         * the clone */

        SV ** dest = (*output == NULL) ? output : &u;
        *dest = invlist_clone(b);
        if (complement_b) {
            _invlist_invert(*dest);
        }

        if (dest == &u) {
            invlist_replace_list_destroys_src(*output, u);
            SvREFCNT_dec_NN(u);
        }

	return;
    }

    /* Here both lists exist and are non-empty */
    array_a = invlist_array(a);
    array_b = invlist_array(b);

    /* If are to take the union of 'a' with the complement of b, set it
     * up so are looking at b's complement. */
    if (complement_b) {

	/* To complement, we invert: if the first element is 0, remove it.  To
	 * do this, we just pretend the array starts one later */
        if (array_b[0] == 0) {
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

    /* Size the union for the worst case: that the sets are completely
     * disjoint */
    u = _new_invlist(len_a + len_b);

    /* Will contain U+0000 if either component does */
    array_u = _invlist_array_init(u, (    len_a > 0 && array_a[0] == 0)
                                      || (len_b > 0 && array_b[0] == 0));

    /* Go through each input list item by item, stopping when have exhausted
     * one of them */
    while (i_a < len_a && i_b < len_b) {
	UV cp;	    /* The element to potentially add to the union's array */
	bool cp_in_set;   /* is it in the the input list's set or not */

	/* We need to take one or the other of the two inputs for the union.
	 * Since we are merging two sorted lists, we take the smaller of the
         * next items.  In case of a tie, we take first the one that is in its
         * set.  If we first took the one not in its set, it would decrement
         * the count, possibly to 0 which would cause it to be output as ending
         * the range, and the next time through we would take the same number,
         * and output it again as beginning the next range.  By doing it the
         * opposite way, there is no possibility that the count will be
         * momentarily decremented to 0, and thus the two adjoining ranges will
         * be seamlessly merged.  (In a tie and both are in the set or both not
         * in the set, it doesn't matter which we take first.) */
	if (       array_a[i_a] < array_b[i_b]
	    || (   array_a[i_a] == array_b[i_b]
		&& ELEMENT_RANGE_MATCHES_INVLIST(i_a)))
	{
	    cp_in_set = ELEMENT_RANGE_MATCHES_INVLIST(i_a);
	    cp = array_a[i_a++];
	}
	else {
	    cp_in_set = ELEMENT_RANGE_MATCHES_INVLIST(i_b);
	    cp = array_b[i_b++];
	}

	/* Here, have chosen which of the two inputs to look at.  Only output
	 * if the running count changes to/from 0, which marks the
	 * beginning/end of a range that's in the set */
	if (cp_in_set) {
	    if (count == 0) {
		array_u[i_u++] = cp;
	    }
	    count++;
	}
	else {
	    count--;
	    if (count == 0) {
		array_u[i_u++] = cp;
	    }
	}
    }


    /* The loop above increments the index into exactly one of the input lists
     * each iteration, and ends when either index gets to its list end.  That
     * means the other index is lower than its end, and so something is
     * remaining in that one.  We decrement 'count', as explained below, if
     * that list is in its set.  (i_a and i_b each currently index the element
     * beyond the one we care about.) */
    if (   (i_a != len_a && PREV_RANGE_MATCHES_INVLIST(i_a))
	|| (i_b != len_b && PREV_RANGE_MATCHES_INVLIST(i_b)))
    {
	count--;
    }

    /* Above we decremented 'count' if the list that had unexamined elements in
     * it was in its set.  This has made it so that 'count' being non-zero
     * means there isn't anything left to output; and 'count' equal to 0 means
     * that what is left to output is precisely that which is left in the
     * non-exhausted input list.
     *
     * To see why, note first that the exhausted input obviously has nothing
     * left to add to the union.  If it was in its set at its end, that means
     * the set extends from here to the platform's infinity, and hence so does
     * the union and the non-exhausted set is irrelevant.  The exhausted set
     * also contributed 1 to 'count'.  If 'count' was 2, it got decremented to
     * 1, but if it was 1, the non-exhausted set wasn't in its set, and so
     * 'count' remains at 1.  This is consistent with the decremented 'count'
     * != 0 meaning there's nothing left to add to the union.
     *
     * But if the exhausted input wasn't in its set, it contributed 0 to
     * 'count', and the rest of the union will be whatever the other input is.
     * If 'count' was 0, neither list was in its set, and 'count' remains 0;
     * otherwise it gets decremented to 0.  This is consistent with 'count'
     * == 0 meaning the remainder of the union is whatever is left in the
     * non-exhausted list. */
    if (count != 0) {
        len_u = i_u;
    }
    else {
        IV copy_count = len_a - i_a;
        if (copy_count > 0) {   /* The non-exhausted input is 'a' */
	    Copy(array_a + i_a, array_u + i_u, copy_count, UV);
        }
        else { /* The non-exhausted input is b */
            copy_count = len_b - i_b;
	    Copy(array_b + i_b, array_u + i_u, copy_count, UV);
        }
        len_u = i_u + copy_count;
    }

    /* Set the result to the final length, which can change the pointer to
     * array_u, so re-find it.  (Note that it is unlikely that this will
     * change, as we are shrinking the space, not enlarging it) */
    if (len_u != _invlist_len(u)) {
	invlist_set_len(u, len_u, *get_invlist_offset_addr(u));
	invlist_trim(u);
	array_u = invlist_array(u);
    }

    if (*output == NULL) {  /* Simply return the new inversion list */
        *output = u;
    }
    else {
        /* Otherwise, overwrite the inversion list that was in '*output'.  We
         * could instead free '*output', and then set it to 'u', but experience
         * has shown [perl #127392] that if the input is a mortal, we can get a
         * huge build-up of these during regex compilation before they get
         * freed. */
        invlist_replace_list_destroys_src(*output, u);
        SvREFCNT_dec_NN(u);
    }

    return;
}