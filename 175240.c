static int ovs_flow_cmd_fill_info(struct sw_flow *flow, struct datapath *dp,
				  struct sk_buff *skb, u32 portid,
				  u32 seq, u32 flags, u8 cmd)
{
	const int skb_orig_len = skb->len;
	struct nlattr *start;
	struct ovs_flow_stats stats;
	__be16 tcp_flags;
	unsigned long used;
	struct ovs_header *ovs_header;
	struct nlattr *nla;
	int err;

	ovs_header = genlmsg_put(skb, portid, seq, &dp_flow_genl_family, flags, cmd);
	if (!ovs_header)
		return -EMSGSIZE;

	ovs_header->dp_ifindex = get_dpifindex(dp);

	/* Fill flow key. */
	nla = nla_nest_start(skb, OVS_FLOW_ATTR_KEY);
	if (!nla)
		goto nla_put_failure;

	err = ovs_nla_put_flow(&flow->unmasked_key, &flow->unmasked_key, skb);
	if (err)
		goto error;
	nla_nest_end(skb, nla);

	nla = nla_nest_start(skb, OVS_FLOW_ATTR_MASK);
	if (!nla)
		goto nla_put_failure;

	err = ovs_nla_put_flow(&flow->key, &flow->mask->key, skb);
	if (err)
		goto error;

	nla_nest_end(skb, nla);

	ovs_flow_stats_get(flow, &stats, &used, &tcp_flags);
	if (used &&
	    nla_put_u64(skb, OVS_FLOW_ATTR_USED, ovs_flow_used_time(used)))
		goto nla_put_failure;

	if (stats.n_packets &&
	    nla_put(skb, OVS_FLOW_ATTR_STATS, sizeof(struct ovs_flow_stats), &stats))
		goto nla_put_failure;

	if ((u8)ntohs(tcp_flags) &&
	     nla_put_u8(skb, OVS_FLOW_ATTR_TCP_FLAGS, (u8)ntohs(tcp_flags)))
		goto nla_put_failure;

	/* If OVS_FLOW_ATTR_ACTIONS doesn't fit, skip dumping the actions if
	 * this is the first flow to be dumped into 'skb'.  This is unusual for
	 * Netlink but individual action lists can be longer than
	 * NLMSG_GOODSIZE and thus entirely undumpable if we didn't do this.
	 * The userspace caller can always fetch the actions separately if it
	 * really wants them.  (Most userspace callers in fact don't care.)
	 *
	 * This can only fail for dump operations because the skb is always
	 * properly sized for single flows.
	 */
	start = nla_nest_start(skb, OVS_FLOW_ATTR_ACTIONS);
	if (start) {
		const struct sw_flow_actions *sf_acts;

		sf_acts = rcu_dereference_ovsl(flow->sf_acts);

		err = ovs_nla_put_actions(sf_acts->actions,
					  sf_acts->actions_len, skb);
		if (!err)
			nla_nest_end(skb, start);
		else {
			if (skb_orig_len)
				goto error;

			nla_nest_cancel(skb, start);
		}
	} else if (skb_orig_len)
		goto nla_put_failure;

	return genlmsg_end(skb, ovs_header);

nla_put_failure:
	err = -EMSGSIZE;
error:
	genlmsg_cancel(skb, ovs_header);
	return err;
}