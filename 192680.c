Perl__invlist_intersection_maybe_complement_2nd(pTHX_ SV* const a, SV* const b,
                                               const bool complement_b, SV** i)
{
    /* Take the intersection of two inversion lists and point '*i' to it.  On
     * input, '*i' MUST POINT TO NULL OR TO AN SV* INVERSION LIST (possibly
     * even 'a' or 'b').  If to an inversion list, the contents of the original
     * list will be replaced by the intersection.  The first list, 'a', may be
     * NULL, in which case '*i' will be an empty list.  If 'complement_b' is
     * TRUE, the result will be the intersection of 'a' and the complement (or
     * inversion) of 'b' instead of 'b' directly.
     *
     * The basis for this comes from "Unicode Demystified" Chapter 13 by
     * Richard Gillam, published by Addison-Wesley, and explained at some
     * length there.  The preface says to incorporate its examples into your
     * code at your own risk.  In fact, it had bugs
     *
     * The algorithm is like a merge sort, and is essentially the same as the
     * union above
     */

    const UV* array_a;		/* a's array */
    const UV* array_b;
    UV len_a;	/* length of a's array */
    UV len_b;

    SV* r;		     /* the resulting intersection */
    UV* array_r;
    UV len_r = 0;

    UV i_a = 0;		    /* current index into a's array */
    UV i_b = 0;
    UV i_r = 0;

    /* running count of how many of the two inputs are postitioned at ranges
     * that are in their sets.  As explained in the algorithm source book,
     * items are stopped accumulating and are output when the count changes
     * to/from 2.  The count is incremented when we start a range that's in an
     * input's set, and decremented when we start a range that's not in a set.
     * Only when it is 2 are we in the intersection. */
    UV count = 0;

    PERL_ARGS_ASSERT__INVLIST_INTERSECTION_MAYBE_COMPLEMENT_2ND;
    assert(a != b);
    assert(*i == NULL || SvTYPE(*i) == SVt_INVLIST);

    /* Special case if either one is empty */
    len_a = (a == NULL) ? 0 : _invlist_len(a);
    if ((len_a == 0) || ((len_b = _invlist_len(b)) == 0)) {
        if (len_a != 0 && complement_b) {

            /* Here, 'a' is not empty, therefore from the enclosing 'if', 'b'
             * must be empty.  Here, also we are using 'b's complement, which
             * hence must be every possible code point.  Thus the intersection
             * is simply 'a'. */

            if (*i == a) {  /* No-op */
                return;
            }

            if (*i == NULL) {
                *i = invlist_clone(a);
                return;
            }

            r = invlist_clone(a);
            invlist_replace_list_destroys_src(*i, r);
            SvREFCNT_dec_NN(r);
            return;
        }

        /* Here, 'a' or 'b' is empty and not using the complement of 'b'.  The
         * intersection must be empty */
        if (*i == NULL) {
            *i = _new_invlist(0);
            return;
        }

        invlist_clear(*i);
	return;
    }

    /* Here both lists exist and are non-empty */
    array_a = invlist_array(a);
    array_b = invlist_array(b);

    /* If are to take the intersection of 'a' with the complement of b, set it
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

    /* Size the intersection for the worst case: that the intersection ends up
     * fragmenting everything to be completely disjoint */
    r= _new_invlist(len_a + len_b);

    /* Will contain U+0000 iff both components do */
    array_r = _invlist_array_init(r,    len_a > 0 && array_a[0] == 0
                                     && len_b > 0 && array_b[0] == 0);

    /* Go through each list item by item, stopping when have exhausted one of
     * them */
    while (i_a < len_a && i_b < len_b) {
	UV cp;	    /* The element to potentially add to the intersection's
		       array */
	bool cp_in_set;	/* Is it in the input list's set or not */

	/* We need to take one or the other of the two inputs for the
	 * intersection.  Since we are merging two sorted lists, we take the
         * smaller of the next items.  In case of a tie, we take first the one
         * that is not in its set (a difference from the union algorithm).  If
         * we first took the one in its set, it would increment the count,
         * possibly to 2 which would cause it to be output as starting a range
         * in the intersection, and the next time through we would take that
         * same number, and output it again as ending the set.  By doing the
         * opposite of this, there is no possibility that the count will be
         * momentarily incremented to 2.  (In a tie and both are in the set or
         * both not in the set, it doesn't matter which we take first.) */
	if (       array_a[i_a] < array_b[i_b]
	    || (   array_a[i_a] == array_b[i_b]
		&& ! ELEMENT_RANGE_MATCHES_INVLIST(i_a)))
	{
	    cp_in_set = ELEMENT_RANGE_MATCHES_INVLIST(i_a);
	    cp = array_a[i_a++];
	}
	else {
	    cp_in_set = ELEMENT_RANGE_MATCHES_INVLIST(i_b);
	    cp= array_b[i_b++];
	}

	/* Here, have chosen which of the two inputs to look at.  Only output
	 * if the running count changes to/from 2, which marks the
	 * beginning/end of a range that's in the intersection */
	if (cp_in_set) {
	    count++;
	    if (count == 2) {
		array_r[i_r++] = cp;
	    }
	}
	else {
	    if (count == 2) {
		array_r[i_r++] = cp;
	    }
	    count--;
	}

    }

    /* The loop above increments the index into exactly one of the input lists
     * each iteration, and ends when either index gets to its list end.  That
     * means the other index is lower than its end, and so something is
     * remaining in that one.  We increment 'count', as explained below, if the
     * exhausted list was in its set.  (i_a and i_b each currently index the
     * element beyond the one we care about.) */
    if (   (i_a == len_a && PREV_RANGE_MATCHES_INVLIST(i_a))
        || (i_b == len_b && PREV_RANGE_MATCHES_INVLIST(i_b)))
    {
	count++;
    }

    /* Above we incremented 'count' if the exhausted list was in its set.  This
     * has made it so that 'count' being below 2 means there is nothing left to
     * output; otheriwse what's left to add to the intersection is precisely
     * that which is left in the non-exhausted input list.
     *
     * To see why, note first that the exhausted input obviously has nothing
     * left to affect the intersection.  If it was in its set at its end, that
     * means the set extends from here to the platform's infinity, and hence
     * anything in the non-exhausted's list will be in the intersection, and
     * anything not in it won't be.  Hence, the rest of the intersection is
     * precisely what's in the non-exhausted list  The exhausted set also
     * contributed 1 to 'count', meaning 'count' was at least 1.  Incrementing
     * it means 'count' is now at least 2.  This is consistent with the
     * incremented 'count' being >= 2 means to add the non-exhausted list to
     * the intersection.
     *
     * But if the exhausted input wasn't in its set, it contributed 0 to
     * 'count', and the intersection can't include anything further; the
     * non-exhausted set is irrelevant.  'count' was at most 1, and doesn't get
     * incremented.  This is consistent with 'count' being < 2 meaning nothing
     * further to add to the intersection. */
    if (count < 2) { /* Nothing left to put in the intersection. */
        len_r = i_r;
    }
    else { /* copy the non-exhausted list, unchanged. */
        IV copy_count = len_a - i_a;
        if (copy_count > 0) {   /* a is the one with stuff left */
	    Copy(array_a + i_a, array_r + i_r, copy_count, UV);
        }
        else {  /* b is the one with stuff left */
            copy_count = len_b - i_b;
	    Copy(array_b + i_b, array_r + i_r, copy_count, UV);
        }
        len_r = i_r + copy_count;
    }

    /* Set the result to the final length, which can change the pointer to
     * array_r, so re-find it.  (Note that it is unlikely that this will
     * change, as we are shrinking the space, not enlarging it) */
    if (len_r != _invlist_len(r)) {
	invlist_set_len(r, len_r, *get_invlist_offset_addr(r));
	invlist_trim(r);
	array_r = invlist_array(r);
    }

    if (*i == NULL) { /* Simply return the calculated intersection */
        *i = r;
    }
    else { /* Otherwise, replace the existing inversion list in '*i'.  We could
              instead free '*i', and then set it to 'r', but experience has
              shown [perl #127392] that if the input is a mortal, we can get a
              huge build-up of these during regex compilation before they get
              freed. */
        if (len_r) {
            invlist_replace_list_destroys_src(*i, r);
        }
        else {
            invlist_clear(*i);
        }
        SvREFCNT_dec_NN(r);
    }

    return;
}