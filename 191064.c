struct sk_buff *dccp_ctl_make_reset(struct socket *ctl, struct sk_buff *rcv_skb)
{
	struct dccp_hdr *rxdh = dccp_hdr(rcv_skb), *dh;
	struct dccp_skb_cb *dcb = DCCP_SKB_CB(rcv_skb);
	const u32 dccp_hdr_reset_len = sizeof(struct dccp_hdr) +
				       sizeof(struct dccp_hdr_ext) +
				       sizeof(struct dccp_hdr_reset);
	struct dccp_hdr_reset *dhr;
	struct sk_buff *skb;

	skb = alloc_skb(ctl->sk->sk_prot->max_header, GFP_ATOMIC);
	if (skb == NULL)
		return NULL;

	skb_reserve(skb, ctl->sk->sk_prot->max_header);

	/* Swap the send and the receive. */
	dh = dccp_zeroed_hdr(skb, dccp_hdr_reset_len);
	dh->dccph_type	= DCCP_PKT_RESET;
	dh->dccph_sport	= rxdh->dccph_dport;
	dh->dccph_dport	= rxdh->dccph_sport;
	dh->dccph_doff	= dccp_hdr_reset_len / 4;
	dh->dccph_x	= 1;

	dhr = dccp_hdr_reset(skb);
	dhr->dccph_reset_code = dcb->dccpd_reset_code;

	switch (dcb->dccpd_reset_code) {
	case DCCP_RESET_CODE_PACKET_ERROR:
		dhr->dccph_reset_data[0] = rxdh->dccph_type;
		break;
	case DCCP_RESET_CODE_OPTION_ERROR:	/* fall through */
	case DCCP_RESET_CODE_MANDATORY_ERROR:
		memcpy(dhr->dccph_reset_data, dcb->dccpd_reset_data, 3);
		break;
	}
	/*
	 * From RFC 4340, 8.3.1:
	 *   If P.ackno exists, set R.seqno := P.ackno + 1.
	 *   Else set R.seqno := 0.
	 */
	if (dcb->dccpd_ack_seq != DCCP_PKT_WITHOUT_ACK_SEQ)
		dccp_hdr_set_seq(dh, ADD48(dcb->dccpd_ack_seq, 1));
	dccp_hdr_set_ack(dccp_hdr_ack_bits(skb), dcb->dccpd_seq);

	dccp_csum_outgoing(skb);
	return skb;
}