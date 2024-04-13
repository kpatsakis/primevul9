void ovs_nla_free_flow_actions(struct sw_flow_actions *sf_acts)
{
	if (!sf_acts)
		return;

	ovs_nla_free_nested_actions(sf_acts->actions, sf_acts->actions_len);
	kfree(sf_acts);
}