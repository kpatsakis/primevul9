static int ovs_flow_cmd_get(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr **a = info->attrs;
	struct ovs_header *ovs_header = info->userhdr;
	struct sw_flow_key key;
	struct sk_buff *reply;
	struct sw_flow *flow;
	struct datapath *dp;
	struct sw_flow_match match;
	int err;

	if (!a[OVS_FLOW_ATTR_KEY]) {
		OVS_NLERR("Flow get message rejected, Key attribute missing.\n");
		return -EINVAL;
	}

	ovs_match_init(&match, &key, NULL);
	err = ovs_nla_get_match(&match, NULL, a[OVS_FLOW_ATTR_KEY], NULL);
	if (err)
		return err;

	ovs_lock();
	dp = get_dp(sock_net(skb->sk), ovs_header->dp_ifindex);
	if (!dp) {
		err = -ENODEV;
		goto unlock;
	}

	flow = ovs_flow_tbl_lookup(&dp->table, &key);
	if (!flow || !ovs_flow_cmp_unmasked_key(flow, &match)) {
		err = -ENOENT;
		goto unlock;
	}

	reply = ovs_flow_cmd_build_info(flow, dp, info, OVS_FLOW_CMD_NEW);
	if (IS_ERR(reply)) {
		err = PTR_ERR(reply);
		goto unlock;
	}

	ovs_unlock();
	return genlmsg_reply(reply, info);
unlock:
	ovs_unlock();
	return err;
}