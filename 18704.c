static int xfrm_policy_match(const struct xfrm_policy *pol,
			     const struct flowi *fl,
			     u8 type, u16 family, int dir, u32 if_id)
{
	const struct xfrm_selector *sel = &pol->selector;
	int ret = -ESRCH;
	bool match;

	if (pol->family != family ||
	    pol->if_id != if_id ||
	    (fl->flowi_mark & pol->mark.m) != pol->mark.v ||
	    pol->type != type)
		return ret;

	match = xfrm_selector_match(sel, fl, family);
	if (match)
		ret = security_xfrm_policy_lookup(pol->security, fl->flowi_secid);
	return ret;
}