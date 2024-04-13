Perl__invlist_search(SV* const invlist, const UV cp)
{
    /* Searches the inversion list for the entry that contains the input code
     * point <cp>.  If <cp> is not in the list, -1 is returned.  Otherwise, the
     * return value is the index into the list's array of the range that
     * contains <cp>, that is, 'i' such that
     *	array[i] <= cp < array[i+1]
     */

    IV low = 0;
    IV mid;
    IV high = _invlist_len(invlist);
    const IV highest_element = high - 1;
    const UV* array;

    PERL_ARGS_ASSERT__INVLIST_SEARCH;

    /* If list is empty, return failure. */
    if (high == 0) {
	return -1;
    }

    /* (We can't get the array unless we know the list is non-empty) */
    array = invlist_array(invlist);

    mid = invlist_previous_index(invlist);
    assert(mid >=0);
    if (mid > highest_element) {
        mid = highest_element;
    }

    /* <mid> contains the cache of the result of the previous call to this
     * function (0 the first time).  See if this call is for the same result,
     * or if it is for mid-1.  This is under the theory that calls to this
     * function will often be for related code points that are near each other.
     * And benchmarks show that caching gives better results.  We also test
     * here if the code point is within the bounds of the list.  These tests
     * replace others that would have had to be made anyway to make sure that
     * the array bounds were not exceeded, and these give us extra information
     * at the same time */
    if (cp >= array[mid]) {
        if (cp >= array[highest_element]) {
            return highest_element;
        }

        /* Here, array[mid] <= cp < array[highest_element].  This means that
         * the final element is not the answer, so can exclude it; it also
         * means that <mid> is not the final element, so can refer to 'mid + 1'
         * safely */
        if (cp < array[mid + 1]) {
            return mid;
        }
        high--;
        low = mid + 1;
    }
    else { /* cp < aray[mid] */
        if (cp < array[0]) { /* Fail if outside the array */
            return -1;
        }
        high = mid;
        if (cp >= array[mid - 1]) {
            goto found_entry;
        }
    }

    /* Binary search.  What we are looking for is <i> such that
     *	array[i] <= cp < array[i+1]
     * The loop below converges on the i+1.  Note that there may not be an
     * (i+1)th element in the array, and things work nonetheless */
    while (low < high) {
	mid = (low + high) / 2;
        assert(mid <= highest_element);
	if (array[mid] <= cp) { /* cp >= array[mid] */
	    low = mid + 1;

	    /* We could do this extra test to exit the loop early.
	    if (cp < array[low]) {
		return mid;
	    }
	    */
	}
	else { /* cp < array[mid] */
	    high = mid;
	}
    }

  found_entry:
    high--;
    invlist_set_previous_index(invlist, high);
    return high;
}