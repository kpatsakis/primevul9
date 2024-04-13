mISDN_send(struct mISDNchannel *ch, struct sk_buff *skb)
{
	struct mISDN_sock *msk;
	int	err;

	msk = container_of(ch, struct mISDN_sock, ch);
	if (*debug & DEBUG_SOCKET)
		printk(KERN_DEBUG "%s len %d %p\n", __func__, skb->len, skb);
	if (msk->sk.sk_state == MISDN_CLOSED)
		return -EUNATCH;
	__net_timestamp(skb);
	err = sock_queue_rcv_skb(&msk->sk, skb);
	if (err)
		printk(KERN_WARNING "%s: error %d\n", __func__, err);
	return err;
}