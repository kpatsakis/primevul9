int ovs_nla_put_actions(const struct nlattr *attr, int len, struct sk_buff *skb)
{
	const struct nlattr *a;
	int rem, err;

	nla_for_each_attr(a, attr, len, rem) {
		int type = nla_type(a);

		switch (type) {
		case OVS_ACTION_ATTR_SET:
			err = set_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SET_TO_MASKED:
			err = masked_set_action_to_set_action_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SAMPLE:
			err = sample_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_CT:
			err = ovs_ct_action_to_attr(nla_data(a), skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_CLONE:
			err = clone_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_CHECK_PKT_LEN:
			err = check_pkt_len_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_DEC_TTL:
			err = dec_ttl_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		default:
			if (nla_put(skb, type, nla_len(a), nla_data(a)))
				return -EMSGSIZE;
			break;
		}
	}

	return 0;
}