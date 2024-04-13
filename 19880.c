static void ovs_nla_free_sample_action(const struct nlattr *action)
{
	const struct nlattr *a = nla_data(action);
	int rem = nla_len(action);

	switch (nla_type(a)) {
	case OVS_SAMPLE_ATTR_ARG:
		/* The real list of actions follows this attribute. */
		a = nla_next(a, &rem);
		ovs_nla_free_nested_actions(a, rem);
		break;
	}
}