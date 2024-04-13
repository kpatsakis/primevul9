static void crypto_netlink_rcv(struct sk_buff *skb)
{
	mutex_lock(&crypto_cfg_mutex);
	netlink_rcv_skb(skb, &crypto_user_rcv_msg);
	mutex_unlock(&crypto_cfg_mutex);
}