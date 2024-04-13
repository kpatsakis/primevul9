static bool hard_acs_rdy_or_timeout(struct temac_local *lp, ktime_t timeout)
{
	ktime_t cur = ktime_get();

	return hard_acs_rdy(lp) || ktime_after(cur, timeout);
}