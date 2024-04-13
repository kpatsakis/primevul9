static void bond_fold_stats(struct rtnl_link_stats64 *_res,
			    const struct rtnl_link_stats64 *_new,
			    const struct rtnl_link_stats64 *_old)
{
	const u64 *new = (const u64 *)_new;
	const u64 *old = (const u64 *)_old;
	u64 *res = (u64 *)_res;
	int i;

	for (i = 0; i < sizeof(*_res) / sizeof(u64); i++) {
		u64 nv = new[i];
		u64 ov = old[i];
		s64 delta = nv - ov;

		/* detects if this particular field is 32bit only */
		if (((nv | ov) >> 32) == 0)
			delta = (s64)(s32)((u32)nv - (u32)ov);

		/* filter anomalies, some drivers reset their stats
		 * at down/up events.
		 */
		if (delta > 0)
			res[i] += delta;
	}
}