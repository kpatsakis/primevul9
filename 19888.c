static void ovs_nla_free_check_pkt_len_action(const struct nlattr *action)
{
	const struct nlattr *a;
	int rem;

	nla_for_each_nested(a, action, rem) {
		switch (nla_type(a)) {
		case OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL:
		case OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER:
			ovs_nla_free_nested_actions(nla_data(a), nla_len(a));
			break;
		}
	}
}