void skb_tstamp_tx(struct sk_buff *orig_skb,
		struct skb_shared_hwtstamps *hwtstamps)
{
	struct sock *sk = orig_skb->sk;
	struct sock_exterr_skb *serr;
	struct sk_buff *skb;
	int err;

	if (!sk)
		return;

	if (hwtstamps) {
		*skb_hwtstamps(orig_skb) =
			*hwtstamps;
	} else {
		/*
		 * no hardware time stamps available,
		 * so keep the shared tx_flags and only
		 * store software time stamp
		 */
		orig_skb->tstamp = ktime_get_real();
	}

	skb = skb_clone(orig_skb, GFP_ATOMIC);
	if (!skb)
		return;

	serr = SKB_EXT_ERR(skb);
	memset(serr, 0, sizeof(*serr));
	serr->ee.ee_errno = ENOMSG;
	serr->ee.ee_origin = SO_EE_ORIGIN_TIMESTAMPING;

	err = sock_queue_err_skb(sk, skb);

	if (err)
		kfree_skb(skb);
}