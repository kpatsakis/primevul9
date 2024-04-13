static int ovs_dp_cmd_del(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *reply;
	struct datapath *dp;
	int err;

	ovs_lock();
	dp = lookup_datapath(sock_net(skb->sk), info->userhdr, info->attrs);
	err = PTR_ERR(dp);
	if (IS_ERR(dp))
		goto unlock;

	reply = ovs_dp_cmd_build_info(dp, info, OVS_DP_CMD_DEL);
	err = PTR_ERR(reply);
	if (IS_ERR(reply))
		goto unlock;

	__dp_destroy(dp);
	ovs_unlock();

	ovs_notify(&dp_datapath_genl_family, reply, info);

	return 0;
unlock:
	ovs_unlock();
	return err;
}