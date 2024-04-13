static int check_pkt_len_action_to_attr(const struct nlattr *attr,
					struct sk_buff *skb)
{
	struct nlattr *start, *ac_start = NULL;
	const struct check_pkt_len_arg *arg;
	const struct nlattr *a, *cpl_arg;
	int err = 0, rem = nla_len(attr);

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_CHECK_PKT_LEN);
	if (!start)
		return -EMSGSIZE;

	/* The first nested attribute in 'attr' is always
	 * 'OVS_CHECK_PKT_LEN_ATTR_ARG'.
	 */
	cpl_arg = nla_data(attr);
	arg = nla_data(cpl_arg);

	if (nla_put_u16(skb, OVS_CHECK_PKT_LEN_ATTR_PKT_LEN, arg->pkt_len)) {
		err = -EMSGSIZE;
		goto out;
	}

	/* Second nested attribute in 'attr' is always
	 * 'OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL'.
	 */
	a = nla_next(cpl_arg, &rem);
	ac_start =  nla_nest_start_noflag(skb,
					  OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL);
	if (!ac_start) {
		err = -EMSGSIZE;
		goto out;
	}

	err = ovs_nla_put_actions(nla_data(a), nla_len(a), skb);
	if (err) {
		nla_nest_cancel(skb, ac_start);
		goto out;
	} else {
		nla_nest_end(skb, ac_start);
	}

	/* Third nested attribute in 'attr' is always
	 * OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER.
	 */
	a = nla_next(a, &rem);
	ac_start =  nla_nest_start_noflag(skb,
					  OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER);
	if (!ac_start) {
		err = -EMSGSIZE;
		goto out;
	}

	err = ovs_nla_put_actions(nla_data(a), nla_len(a), skb);
	if (err) {
		nla_nest_cancel(skb, ac_start);
		goto out;
	} else {
		nla_nest_end(skb, ac_start);
	}

	nla_nest_end(skb, start);
	return 0;

out:
	nla_nest_cancel(skb, start);
	return err;
}