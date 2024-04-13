int temac_indirect_busywait(struct temac_local *lp)
{
	ktime_t timeout = ktime_add_ns(ktime_get(), HARD_ACS_RDY_POLL_NS);

	spin_until_cond(hard_acs_rdy_or_timeout(lp, timeout));
	if (WARN_ON(!hard_acs_rdy(lp)))
		return -ETIMEDOUT;
	else
		return 0;
}