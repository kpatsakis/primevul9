static int calc_wheel_index(unsigned long expires, unsigned long clk)
{
	unsigned long delta = expires - clk;
	unsigned int idx;

	if (delta < LVL_START(1)) {
		idx = calc_index(expires, 0);
	} else if (delta < LVL_START(2)) {
		idx = calc_index(expires, 1);
	} else if (delta < LVL_START(3)) {
		idx = calc_index(expires, 2);
	} else if (delta < LVL_START(4)) {
		idx = calc_index(expires, 3);
	} else if (delta < LVL_START(5)) {
		idx = calc_index(expires, 4);
	} else if (delta < LVL_START(6)) {
		idx = calc_index(expires, 5);
	} else if (delta < LVL_START(7)) {
		idx = calc_index(expires, 6);
	} else if (LVL_DEPTH > 8 && delta < LVL_START(8)) {
		idx = calc_index(expires, 7);
	} else if ((long) delta < 0) {
		idx = clk & LVL_MASK;
	} else {
		/*
		 * Force expire obscene large timeouts to expire at the
		 * capacity limit of the wheel.
		 */
		if (delta >= WHEEL_TIMEOUT_CUTOFF)
			expires = clk + WHEEL_TIMEOUT_MAX;

		idx = calc_index(expires, LVL_DEPTH - 1);
	}
	return idx;
}