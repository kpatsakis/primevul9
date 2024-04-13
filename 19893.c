static inline int add_nested_action_start(struct sw_flow_actions **sfa,
					  int attrtype, bool log)
{
	int used = (*sfa)->actions_len;
	int err;

	err = ovs_nla_add_action(sfa, attrtype, NULL, 0, log);
	if (err)
		return err;

	return used;
}