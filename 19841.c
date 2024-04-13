static int clone_action_to_attr(const struct nlattr *attr,
				struct sk_buff *skb)
{
	struct nlattr *start;
	int err = 0, rem = nla_len(attr);

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_CLONE);
	if (!start)
		return -EMSGSIZE;

	/* Skipping the OVS_CLONE_ATTR_EXEC that is always the first attribute. */
	attr = nla_next(nla_data(attr), &rem);
	err = ovs_nla_put_actions(attr, rem, skb);

	if (err)
		nla_nest_cancel(skb, start);
	else
		nla_nest_end(skb, start);

	return err;
}