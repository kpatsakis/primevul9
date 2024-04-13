static int masked_set_action_to_set_action_attr(const struct nlattr *a,
						struct sk_buff *skb)
{
	const struct nlattr *ovs_key = nla_data(a);
	struct nlattr *nla;
	size_t key_len = nla_len(ovs_key) / 2;

	/* Revert the conversion we did from a non-masked set action to
	 * masked set action.
	 */
	nla = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_SET);
	if (!nla)
		return -EMSGSIZE;

	if (nla_put(skb, nla_type(ovs_key), key_len, nla_data(ovs_key)))
		return -EMSGSIZE;

	nla_nest_end(skb, nla);
	return 0;
}