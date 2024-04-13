static int ovs_dp_cmd_set(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *reply;
	struct datapath *dp;
	int err;

	ovs_lock();
	dp = lookup_datapath(sock_net(skb->sk), info->userhdr, info->attrs);
	err = PTR_ERR(dp);
	if (IS_ERR(dp))
		goto unlock;

	ovs_dp_change(dp, info->attrs);

	reply = ovs_dp_cmd_build_info(dp, info, OVS_DP_CMD_NEW);
	if (IS_ERR(reply)) {
		err = PTR_ERR(reply);
		genl_set_err(&dp_datapath_genl_family, sock_net(skb->sk), 0,
			     0, err);
		err = 0;
		goto unlock;
	}

	ovs_unlock();
	ovs_notify(&dp_datapath_genl_family, reply, info);

	return 0;
unlock:
	ovs_unlock();
	return err;
}