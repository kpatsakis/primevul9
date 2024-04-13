static void ovs_nla_free_dec_ttl_action(const struct nlattr *action)
{
	const struct nlattr *a = nla_data(action);

	switch (nla_type(a)) {
	case OVS_DEC_TTL_ATTR_ACTION:
		ovs_nla_free_nested_actions(nla_data(a), nla_len(a));
		break;
	}
}