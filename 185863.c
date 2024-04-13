static int ip6_route_multipath(struct fib6_config *cfg, int add)
{
	struct fib6_config r_cfg;
	struct rtnexthop *rtnh;
	int remaining;
	int attrlen;
	int err = 0, last_err = 0;

beginning:
	rtnh = (struct rtnexthop *)cfg->fc_mp;
	remaining = cfg->fc_mp_len;

	/* Parse a Multipath Entry */
	while (rtnh_ok(rtnh, remaining)) {
		memcpy(&r_cfg, cfg, sizeof(*cfg));
		if (rtnh->rtnh_ifindex)
			r_cfg.fc_ifindex = rtnh->rtnh_ifindex;

		attrlen = rtnh_attrlen(rtnh);
		if (attrlen > 0) {
			struct nlattr *nla, *attrs = rtnh_attrs(rtnh);

			nla = nla_find(attrs, attrlen, RTA_GATEWAY);
			if (nla) {
				nla_memcpy(&r_cfg.fc_gateway, nla, 16);
				r_cfg.fc_flags |= RTF_GATEWAY;
			}
		}
		err = add ? ip6_route_add(&r_cfg) : ip6_route_del(&r_cfg);
		if (err) {
			last_err = err;
			/* If we are trying to remove a route, do not stop the
			 * loop when ip6_route_del() fails (because next hop is
			 * already gone), we should try to remove all next hops.
			 */
			if (add) {
				/* If add fails, we should try to delete all
				 * next hops that have been already added.
				 */
				add = 0;
				goto beginning;
			}
		}
		/* Because each route is added like a single route we remove
		 * this flag after the first nexthop (if there is a collision,
		 * we have already fail to add the first nexthop:
		 * fib6_add_rt2node() has reject it).
		 */
		cfg->fc_nlinfo.nlh->nlmsg_flags &= ~NLM_F_EXCL;
		rtnh = rtnh_next(rtnh, &remaining);
	}

	return last_err;
}