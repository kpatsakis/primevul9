Perl__add_range_to_invlist(pTHX_ SV* invlist, UV start, UV end)
{
    /* Add the range from 'start' to 'end' inclusive to the inversion list's
     * set.  A pointer to the inversion list is returned.  This may actually be
     * a new list, in which case the passed in one has been destroyed.  The
     * passed-in inversion list can be NULL, in which case a new one is created
     * with just the one range in it.  The new list is not necessarily
     * NUL-terminated.  Space is not freed if the inversion list shrinks as a
     * result of this function.  The gain would not be large, and in many
     * cases, this is called multiple times on a single inversion list, so
     * anything freed may almost immediately be needed again.
     *
     * This used to mostly call the 'union' routine, but that is much more
     * heavyweight than really needed for a single range addition */

    UV* array;              /* The array implementing the inversion list */
    UV len;                 /* How many elements in 'array' */
    SSize_t i_s;            /* index into the invlist array where 'start'
                               should go */
    SSize_t i_e = 0;        /* And the index where 'end' should go */
    UV cur_highest;         /* The highest code point in the inversion list
                               upon entry to this function */

    /* This range becomes the whole inversion list if none already existed */
    if (invlist == NULL) {
	invlist = _new_invlist(2);
        _append_range_to_invlist(invlist, start, end);
        return invlist;
    }

    /* Likewise, if the inversion list is currently empty */
    len = _invlist_len(invlist);
    if (len == 0) {
        _append_range_to_invlist(invlist, start, end);
        return invlist;
    }

    /* Starting here, we have to know the internals of the list */
    array = invlist_array(invlist);

    /* If the new range ends higher than the current highest ... */
    cur_highest = invlist_highest(invlist);
    if (end > cur_highest) {

        /* If the whole range is higher, we can just append it */
        if (start > cur_highest) {
            _append_range_to_invlist(invlist, start, end);
            return invlist;
        }

        /* Otherwise, add the portion that is higher ... */
        _append_range_to_invlist(invlist, cur_highest + 1, end);

        /* ... and continue on below to handle the rest.  As a result of the
         * above append, we know that the index of the end of the range is the
         * final even numbered one of the array.  Recall that the final element
         * always starts a range that extends to infinity.  If that range is in
         * the set (meaning the set goes from here to infinity), it will be an
         * even index, but if it isn't in the set, it's odd, and the final
         * range in the set is one less, which is even. */
        if (end == UV_MAX) {
            i_e = len;
        }
        else {
            i_e = len - 2;
        }
    }

    /* We have dealt with appending, now see about prepending.  If the new
     * range starts lower than the current lowest ... */
    if (start < array[0]) {

        /* Adding something which has 0 in it is somewhat tricky, and uncommon.
         * Let the union code handle it, rather than having to know the
         * trickiness in two code places.  */
        if (UNLIKELY(start == 0)) {
            SV* range_invlist;

            range_invlist = _new_invlist(2);
            _append_range_to_invlist(range_invlist, start, end);

            _invlist_union(invlist, range_invlist, &invlist);

            SvREFCNT_dec_NN(range_invlist);

            return invlist;
        }

        /* If the whole new range comes before the first entry, and doesn't
         * extend it, we have to insert it as an additional range */
        if (end < array[0] - 1) {
            i_s = i_e = -1;
            goto splice_in_new_range;
        }

        /* Here the new range adjoins the existing first range, extending it
         * downwards. */
        array[0] = start;

        /* And continue on below to handle the rest.  We know that the index of
         * the beginning of the range is the first one of the array */
        i_s = 0;
    }
    else { /* Not prepending any part of the new range to the existing list.
            * Find where in the list it should go.  This finds i_s, such that:
            *     invlist[i_s] <= start < array[i_s+1]
            */
        i_s = _invlist_search(invlist, start);
    }

    /* At this point, any extending before the beginning of the inversion list
     * and/or after the end has been done.  This has made it so that, in the
     * code below, each endpoint of the new range is either in a range that is
     * in the set, or is in a gap between two ranges that are.  This means we
     * don't have to worry about exceeding the array bounds.
     *
     * Find where in the list the new range ends (but we can skip this if we
     * have already determined what it is, or if it will be the same as i_s,
     * which we already have computed) */
    if (i_e == 0) {
        i_e = (start == end)
              ? i_s
              : _invlist_search(invlist, end);
    }

    /* Here generally invlist[i_e] <= end < array[i_e+1].  But if invlist[i_e]
     * is a range that goes to infinity there is no element at invlist[i_e+1],
     * so only the first relation holds. */

    if ( ! ELEMENT_RANGE_MATCHES_INVLIST(i_s)) {

        /* Here, the ranges on either side of the beginning of the new range
         * are in the set, and this range starts in the gap between them.
         *
         * The new range extends the range above it downwards if the new range
         * ends at or above that range's start */
        const bool extends_the_range_above = (   end == UV_MAX
                                              || end + 1 >= array[i_s+1]);

        /* The new range extends the range below it upwards if it begins just
         * after where that range ends */
        if (start == array[i_s]) {

            /* If the new range fills the entire gap between the other ranges,
             * they will get merged together.  Other ranges may also get
             * merged, depending on how many of them the new range spans.  In
             * the general case, we do the merge later, just once, after we
             * figure out how many to merge.  But in the case where the new
             * range exactly spans just this one gap (possibly extending into
             * the one above), we do the merge here, and an early exit.  This
             * is done here to avoid having to special case later. */
            if (i_e - i_s <= 1) {

                /* If i_e - i_s == 1, it means that the new range terminates
                 * within the range above, and hence 'extends_the_range_above'
                 * must be true.  (If the range above it extends to infinity,
                 * 'i_s+2' will be above the array's limit, but 'len-i_s-2'
                 * will be 0, so no harm done.) */
                if (extends_the_range_above) {
                    Move(array + i_s + 2, array + i_s, len - i_s - 2, UV);
                    invlist_set_len(invlist,
                                    len - 2,
                                    *(get_invlist_offset_addr(invlist)));
                    return invlist;
                }

                /* Here, i_e must == i_s.  We keep them in sync, as they apply
                 * to the same range, and below we are about to decrement i_s
                 * */
                i_e--;
            }

            /* Here, the new range is adjacent to the one below.  (It may also
             * span beyond the range above, but that will get resolved later.)
             * Extend the range below to include this one. */
            array[i_s] = (end == UV_MAX) ? UV_MAX : end + 1;
            i_s--;
            start = array[i_s];
        }
        else if (extends_the_range_above) {

            /* Here the new range only extends the range above it, but not the
             * one below.  It merges with the one above.  Again, we keep i_e
             * and i_s in sync if they point to the same range */
            if (i_e == i_s) {
                i_e++;
            }
            i_s++;
            array[i_s] = start;
        }
    }

    /* Here, we've dealt with the new range start extending any adjoining
     * existing ranges.
     *
     * If the new range extends to infinity, it is now the final one,
     * regardless of what was there before */
    if (UNLIKELY(end == UV_MAX)) {
        invlist_set_len(invlist, i_s + 1, *(get_invlist_offset_addr(invlist)));
        return invlist;
    }

    /* If i_e started as == i_s, it has also been dealt with,
     * and been updated to the new i_s, which will fail the following if */
    if (! ELEMENT_RANGE_MATCHES_INVLIST(i_e)) {

        /* Here, the ranges on either side of the end of the new range are in
         * the set, and this range ends in the gap between them.
         *
         * If this range is adjacent to (hence extends) the range above it, it
         * becomes part of that range; likewise if it extends the range below,
         * it becomes part of that range */
        if (end + 1 == array[i_e+1]) {
            i_e++;
            array[i_e] = start;
        }
        else if (start <= array[i_e]) {
            array[i_e] = end + 1;
            i_e--;
        }
    }

    if (i_s == i_e) {

        /* If the range fits entirely in an existing range (as possibly already
         * extended above), it doesn't add anything new */
        if (ELEMENT_RANGE_MATCHES_INVLIST(i_s)) {
            return invlist;
        }

        /* Here, no part of the range is in the list.  Must add it.  It will
         * occupy 2 more slots */
      splice_in_new_range:

        invlist_extend(invlist, len + 2);
        array = invlist_array(invlist);
        /* Move the rest of the array down two slots. Don't include any
         * trailing NUL */
        Move(array + i_e + 1, array + i_e + 3, len - i_e - 1, UV);

        /* Do the actual splice */
        array[i_e+1] = start;
        array[i_e+2] = end + 1;
        invlist_set_len(invlist, len + 2, *(get_invlist_offset_addr(invlist)));
        return invlist;
    }

    /* Here the new range crossed the boundaries of a pre-existing range.  The
     * code above has adjusted things so that both ends are in ranges that are
     * in the set.  This means everything in between must also be in the set.
     * Just squash things together */
    Move(array + i_e + 1, array + i_s + 1, len - i_e - 1, UV);
    invlist_set_len(invlist,
                    len - i_e + i_s,
                    *(get_invlist_offset_addr(invlist)));

    return invlist;
}