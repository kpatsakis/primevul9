static struct sk_buff *ovs_flow_cmd_alloc_info(struct sw_flow *flow,
					       struct genl_info *info)
{
	size_t len;

	len = ovs_flow_cmd_msg_size(ovsl_dereference(flow->sf_acts));

	return genlmsg_new_unicast(len, info, GFP_KERNEL);
}