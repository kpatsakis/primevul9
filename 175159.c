static size_t ovs_dp_cmd_msg_size(void)
{
	size_t msgsize = NLMSG_ALIGN(sizeof(struct ovs_header));

	msgsize += nla_total_size(IFNAMSIZ);
	msgsize += nla_total_size(sizeof(struct ovs_dp_stats));
	msgsize += nla_total_size(sizeof(struct ovs_dp_megaflow_stats));
	msgsize += nla_total_size(sizeof(u32)); /* OVS_DP_ATTR_USER_FEATURES */

	return msgsize;
}