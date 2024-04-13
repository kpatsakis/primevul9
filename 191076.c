struct sk_buff *dccp_make_response(struct sock *sk, struct dst_entry *dst,
				   struct request_sock *req)
{
	struct dccp_hdr *dh;
	struct dccp_request_sock *dreq;
	const u32 dccp_header_size = sizeof(struct dccp_hdr) +
				     sizeof(struct dccp_hdr_ext) +
				     sizeof(struct dccp_hdr_response);
	struct sk_buff *skb = sock_wmalloc(sk, sk->sk_prot->max_header, 1,
					   GFP_ATOMIC);
	if (skb == NULL)
		return NULL;

	/* Reserve space for headers. */
	skb_reserve(skb, sk->sk_prot->max_header);

	skb->dst = dst_clone(dst);

	dreq = dccp_rsk(req);
	if (inet_rsk(req)->acked)	/* increase ISS upon retransmission */
		dccp_inc_seqno(&dreq->dreq_iss);
	DCCP_SKB_CB(skb)->dccpd_type = DCCP_PKT_RESPONSE;
	DCCP_SKB_CB(skb)->dccpd_seq  = dreq->dreq_iss;

	if (dccp_insert_options(sk, skb)) {
		kfree_skb(skb);
		return NULL;
	}

	/* Build and checksum header */
	dh = dccp_zeroed_hdr(skb, dccp_header_size);

	dh->dccph_sport	= inet_sk(sk)->sport;
	dh->dccph_dport	= inet_rsk(req)->rmt_port;
	dh->dccph_doff	= (dccp_header_size +
			   DCCP_SKB_CB(skb)->dccpd_opt_len) / 4;
	dh->dccph_type	= DCCP_PKT_RESPONSE;
	dh->dccph_x	= 1;
	dccp_hdr_set_seq(dh, dreq->dreq_iss);
	dccp_hdr_set_ack(dccp_hdr_ack_bits(skb), dreq->dreq_isr);
	dccp_hdr_response(skb)->dccph_resp_service = dreq->dreq_service;

	dccp_csum_outgoing(skb);

	/* We use `acked' to remember that a Response was already sent. */
	inet_rsk(req)->acked = 1;
	DCCP_INC_STATS(DCCP_MIB_OUTSEGS);
	return skb;
}