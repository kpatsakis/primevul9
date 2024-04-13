static int dec_ttl_action_to_attr(const struct nlattr *attr,
				  struct sk_buff *skb)
{
	struct nlattr *start, *action_start;
	const struct nlattr *a;
	int err = 0, rem;

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_DEC_TTL);
	if (!start)
		return -EMSGSIZE;

	nla_for_each_attr(a, nla_data(attr), nla_len(attr), rem) {
		switch (nla_type(a)) {
		case OVS_DEC_TTL_ATTR_ACTION:

			action_start = nla_nest_start_noflag(skb, OVS_DEC_TTL_ATTR_ACTION);
			if (!action_start) {
				err = -EMSGSIZE;
				goto out;
			}

			err = ovs_nla_put_actions(nla_data(a), nla_len(a), skb);
			if (err)
				goto out;

			nla_nest_end(skb, action_start);
			break;

		default:
			/* Ignore all other option to be future compatible */
			break;
		}
	}

	nla_nest_end(skb, start);
	return 0;

out:
	nla_nest_cancel(skb, start);
	return err;
}