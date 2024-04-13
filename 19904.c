static int sample_action_to_attr(const struct nlattr *attr,
				 struct sk_buff *skb)
{
	struct nlattr *start, *ac_start = NULL, *sample_arg;
	int err = 0, rem = nla_len(attr);
	const struct sample_arg *arg;
	struct nlattr *actions;

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_SAMPLE);
	if (!start)
		return -EMSGSIZE;

	sample_arg = nla_data(attr);
	arg = nla_data(sample_arg);
	actions = nla_next(sample_arg, &rem);

	if (nla_put_u32(skb, OVS_SAMPLE_ATTR_PROBABILITY, arg->probability)) {
		err = -EMSGSIZE;
		goto out;
	}

	ac_start = nla_nest_start_noflag(skb, OVS_SAMPLE_ATTR_ACTIONS);
	if (!ac_start) {
		err = -EMSGSIZE;
		goto out;
	}

	err = ovs_nla_put_actions(actions, rem, skb);

out:
	if (err) {
		nla_nest_cancel(skb, ac_start);
		nla_nest_cancel(skb, start);
	} else {
		nla_nest_end(skb, ac_start);
		nla_nest_end(skb, start);
	}

	return err;
}