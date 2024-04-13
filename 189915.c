static bool calc_nx_huge_pages_recovery_period(uint *period)
{
	/*
	 * Use READ_ONCE to get the params, this may be called outside of the
	 * param setters, e.g. by the kthread to compute its next timeout.
	 */
	bool enabled = READ_ONCE(nx_huge_pages);
	uint ratio = READ_ONCE(nx_huge_pages_recovery_ratio);

	if (!enabled || !ratio)
		return false;

	*period = READ_ONCE(nx_huge_pages_recovery_period_ms);
	if (!*period) {
		/* Make sure the period is not less than one second.  */
		ratio = min(ratio, 3600u);
		*period = 60 * 60 * 1000 / ratio;
	}
	return true;
}