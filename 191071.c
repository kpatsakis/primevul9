void dccp_write_xmit(struct sock *sk, int block)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct sk_buff *skb;

	while ((skb = skb_peek(&sk->sk_write_queue))) {
		int err = ccid_hc_tx_send_packet(dp->dccps_hc_tx_ccid, sk, skb);

		if (err > 0) {
			if (!block) {
				sk_reset_timer(sk, &dp->dccps_xmit_timer,
						msecs_to_jiffies(err)+jiffies);
				break;
			} else
				err = dccp_wait_for_ccid(sk, skb, err);
			if (err && err != -EINTR)
				DCCP_BUG("err=%d after dccp_wait_for_ccid", err);
		}

		skb_dequeue(&sk->sk_write_queue);
		if (err == 0) {
			struct dccp_skb_cb *dcb = DCCP_SKB_CB(skb);
			const int len = skb->len;

			if (sk->sk_state == DCCP_PARTOPEN) {
				/* See 8.1.5.  Handshake Completion */
				inet_csk_schedule_ack(sk);
				inet_csk_reset_xmit_timer(sk, ICSK_TIME_DACK,
						  inet_csk(sk)->icsk_rto,
						  DCCP_RTO_MAX);
				dcb->dccpd_type = DCCP_PKT_DATAACK;
			} else if (dccp_ack_pending(sk))
				dcb->dccpd_type = DCCP_PKT_DATAACK;
			else
				dcb->dccpd_type = DCCP_PKT_DATA;

			err = dccp_transmit_skb(sk, skb);
			ccid_hc_tx_packet_sent(dp->dccps_hc_tx_ccid, sk, 0, len);
			if (err)
				DCCP_BUG("err=%d after ccid_hc_tx_packet_sent",
					 err);
		} else {
			dccp_pr_debug("packet discarded due to err=%d\n", err);
			kfree_skb(skb);
		}
	}
}