static struct sk_buff *ovs_flow_cmd_build_info(struct sw_flow *flow,
					       struct datapath *dp,
					       struct genl_info *info,
					       u8 cmd)
{
	struct sk_buff *skb;
	int retval;

	skb = ovs_flow_cmd_alloc_info(flow, info);
	if (!skb)
		return ERR_PTR(-ENOMEM);

	retval = ovs_flow_cmd_fill_info(flow, dp, skb, info->snd_portid,
					info->snd_seq, 0, cmd);
	BUG_ON(retval < 0);
	return skb;
}