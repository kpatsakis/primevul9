static int dccp_transmit_skb(struct sock *sk, struct sk_buff *skb)
{
	if (likely(skb != NULL)) {
		const struct inet_sock *inet = inet_sk(sk);
		const struct inet_connection_sock *icsk = inet_csk(sk);
		struct dccp_sock *dp = dccp_sk(sk);
		struct dccp_skb_cb *dcb = DCCP_SKB_CB(skb);
		struct dccp_hdr *dh;
		/* XXX For now we're using only 48 bits sequence numbers */
		const u32 dccp_header_size = sizeof(*dh) +
					     sizeof(struct dccp_hdr_ext) +
					  dccp_packet_hdr_len(dcb->dccpd_type);
		int err, set_ack = 1;
		u64 ackno = dp->dccps_gsr;

		dccp_inc_seqno(&dp->dccps_gss);

		switch (dcb->dccpd_type) {
		case DCCP_PKT_DATA:
			set_ack = 0;
			/* fall through */
		case DCCP_PKT_DATAACK:
		case DCCP_PKT_RESET:
			break;

		case DCCP_PKT_REQUEST:
			set_ack = 0;
			/* fall through */

		case DCCP_PKT_SYNC:
		case DCCP_PKT_SYNCACK:
			ackno = dcb->dccpd_ack_seq;
			/* fall through */
		default:
			/*
			 * Set owner/destructor: some skbs are allocated via
			 * alloc_skb (e.g. when retransmission may happen).
			 * Only Data, DataAck, and Reset packets should come
			 * through here with skb->sk set.
			 */
			WARN_ON(skb->sk);
			skb_set_owner_w(skb, sk);
			break;
		}

		dcb->dccpd_seq = dp->dccps_gss;

		if (dccp_insert_options(sk, skb)) {
			kfree_skb(skb);
			return -EPROTO;
		}


		/* Build DCCP header and checksum it. */
		dh = dccp_zeroed_hdr(skb, dccp_header_size);
		dh->dccph_type	= dcb->dccpd_type;
		dh->dccph_sport	= inet->sport;
		dh->dccph_dport	= inet->dport;
		dh->dccph_doff	= (dccp_header_size + dcb->dccpd_opt_len) / 4;
		dh->dccph_ccval	= dcb->dccpd_ccval;
		dh->dccph_cscov = dp->dccps_pcslen;
		/* XXX For now we're using only 48 bits sequence numbers */
		dh->dccph_x	= 1;

		dp->dccps_awh = dp->dccps_gss;
		dccp_hdr_set_seq(dh, dp->dccps_gss);
		if (set_ack)
			dccp_hdr_set_ack(dccp_hdr_ack_bits(skb), ackno);

		switch (dcb->dccpd_type) {
		case DCCP_PKT_REQUEST:
			dccp_hdr_request(skb)->dccph_req_service =
							dp->dccps_service;
			break;
		case DCCP_PKT_RESET:
			dccp_hdr_reset(skb)->dccph_reset_code =
							dcb->dccpd_reset_code;
			break;
		}

		icsk->icsk_af_ops->send_check(sk, 0, skb);

		if (set_ack)
			dccp_event_ack_sent(sk);

		DCCP_INC_STATS(DCCP_MIB_OUTSEGS);

		memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));
		err = icsk->icsk_af_ops->queue_xmit(skb, 0);
		return net_xmit_eval(err);
	}
	return -ENOBUFS;
}