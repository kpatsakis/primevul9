static void ovs_dp_reset_user_features(struct sk_buff *skb, struct genl_info *info)
{
	struct datapath *dp;

	dp = lookup_datapath(sock_net(skb->sk), info->userhdr, info->attrs);
	if (IS_ERR(dp))
		return;

	WARN(dp->user_features, "Dropping previously announced user features\n");
	dp->user_features = 0;
}