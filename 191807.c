static void __netlink_deliver_tap(struct sk_buff *skb)
{
	int ret;
	struct netlink_tap *tmp;

	if (!netlink_filter_tap(skb))
		return;

	list_for_each_entry_rcu(tmp, &netlink_tap_all, list) {
		ret = __netlink_deliver_tap_skb(skb, tmp->dev);
		if (unlikely(ret))
			break;
	}
}