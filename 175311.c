static struct sk_buff *ovs_dp_cmd_build_info(struct datapath *dp,
					     struct genl_info *info, u8 cmd)
{
	struct sk_buff *skb;
	int retval;

	skb = genlmsg_new_unicast(ovs_dp_cmd_msg_size(), info, GFP_KERNEL);
	if (!skb)
		return ERR_PTR(-ENOMEM);

	retval = ovs_dp_cmd_fill_info(dp, skb, info->snd_portid, info->snd_seq, 0, cmd);
	if (retval < 0) {
		kfree_skb(skb);
		return ERR_PTR(retval);
	}
	return skb;
}