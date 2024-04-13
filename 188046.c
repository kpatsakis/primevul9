static void tpacket_destruct_skb(struct sk_buff *skb)
{
	struct packet_sock *po = pkt_sk(skb->sk);
	void *ph;

	BUG_ON(skb == NULL);

	if (likely(po->tx_ring.pg_vec)) {
		ph = skb_shinfo(skb)->destructor_arg;
		BUG_ON(__packet_get_status(po, ph) != TP_STATUS_SENDING);
		BUG_ON(atomic_read(&po->tx_ring.pending) == 0);
		atomic_dec(&po->tx_ring.pending);
		__packet_set_status(po, ph, TP_STATUS_AVAILABLE);
	}

	sock_wfree(skb);
}