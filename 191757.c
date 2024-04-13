static void netlink_deliver_tap(struct sk_buff *skb)
{
	rcu_read_lock();

	if (unlikely(!list_empty(&netlink_tap_all)))
		__netlink_deliver_tap(skb);

	rcu_read_unlock();
}