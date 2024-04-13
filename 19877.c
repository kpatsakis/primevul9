static void ovs_nla_free_nested_actions(const struct nlattr *actions, int len)
{
	const struct nlattr *a;
	int rem;

	/* Whenever new actions are added, the need to update this
	 * function should be considered.
	 */
	BUILD_BUG_ON(OVS_ACTION_ATTR_MAX != 23);

	if (!actions)
		return;

	nla_for_each_attr(a, actions, len, rem) {
		switch (nla_type(a)) {
		case OVS_ACTION_ATTR_CHECK_PKT_LEN:
			ovs_nla_free_check_pkt_len_action(a);
			break;

		case OVS_ACTION_ATTR_CLONE:
			ovs_nla_free_clone_action(a);
			break;

		case OVS_ACTION_ATTR_CT:
			ovs_ct_free_action(a);
			break;

		case OVS_ACTION_ATTR_DEC_TTL:
			ovs_nla_free_dec_ttl_action(a);
			break;

		case OVS_ACTION_ATTR_SAMPLE:
			ovs_nla_free_sample_action(a);
			break;

		case OVS_ACTION_ATTR_SET:
			ovs_nla_free_set_action(a);
			break;
		}
	}
}