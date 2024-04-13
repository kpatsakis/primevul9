static int prop_lookup(unsigned long code, unsigned long n)
{
	long l, r, m;

	/*
	 * There is an extra node on the end of the offsets to allow this routine
	 * to work right.  If the index is 0xffff, then there are no nodes for the
	 * property.
	 */
	if ((l = _ucprop_offsets[n]) == 0xffff)
		return 0;

	/*
	 * Locate the next offset that is not 0xffff.  The sentinel at the end of
	 * the array is the max index value.
	 */
	for (m = 1; n + m < _ucprop_size && _ucprop_offsets[n + m] == 0xffff; m++)
		;

	r = _ucprop_offsets[n + m] - 1;

	while (l <= r) {
		/*
		 * Determine a "mid" point and adjust to make sure the mid point is at
		 * the beginning of a range pair.
		 */
		m = (l + r) >> 1;
		m -= (m & 1);
		if (code > _ucprop_ranges[m + 1])
			l = m + 2;
		else if (code < _ucprop_ranges[m])
			r = m - 2;
		else if (code >= _ucprop_ranges[m] && code <= _ucprop_ranges[m + 1])
			return 1;
	}
	return 0;

}