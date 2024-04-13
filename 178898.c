static int pptp_rcv_core(struct sock *sk, struct sk_buff *skb)
{
	struct pppox_sock *po = pppox_sk(sk);
	struct pptp_opt *opt = &po->proto.pptp;
	int headersize, payload_len, seq;
	__u8 *payload;
	struct pptp_gre_header *header;

	if (!(sk->sk_state & PPPOX_CONNECTED)) {
		if (sock_queue_rcv_skb(sk, skb))
			goto drop;
		return NET_RX_SUCCESS;
	}

	header = (struct pptp_gre_header *)(skb->data);
	headersize  = sizeof(*header);

	/* test if acknowledgement present */
	if (PPTP_GRE_IS_A(header->ver)) {
		__u32 ack;

		if (!pskb_may_pull(skb, headersize))
			goto drop;
		header = (struct pptp_gre_header *)(skb->data);

		/* ack in different place if S = 0 */
		ack = PPTP_GRE_IS_S(header->flags) ? header->ack : header->seq;

		ack = ntohl(ack);

		if (ack > opt->ack_recv)
			opt->ack_recv = ack;
		/* also handle sequence number wrap-around  */
		if (WRAPPED(ack, opt->ack_recv))
			opt->ack_recv = ack;
	} else {
		headersize -= sizeof(header->ack);
	}
	/* test if payload present */
	if (!PPTP_GRE_IS_S(header->flags))
		goto drop;

	payload_len = ntohs(header->payload_len);
	seq         = ntohl(header->seq);

	/* check for incomplete packet (length smaller than expected) */
	if (!pskb_may_pull(skb, headersize + payload_len))
		goto drop;

	payload = skb->data + headersize;
	/* check for expected sequence number */
	if (seq < opt->seq_recv + 1 || WRAPPED(opt->seq_recv, seq)) {
		if ((payload[0] == PPP_ALLSTATIONS) && (payload[1] == PPP_UI) &&
				(PPP_PROTOCOL(payload) == PPP_LCP) &&
				((payload[4] == PPP_LCP_ECHOREQ) || (payload[4] == PPP_LCP_ECHOREP)))
			goto allow_packet;
	} else {
		opt->seq_recv = seq;
allow_packet:
		skb_pull(skb, headersize);

		if (payload[0] == PPP_ALLSTATIONS && payload[1] == PPP_UI) {
			/* chop off address/control */
			if (skb->len < 3)
				goto drop;
			skb_pull(skb, 2);
		}

		if ((*skb->data) & 1) {
			/* protocol is compressed */
			skb_push(skb, 1)[0] = 0;
		}

		skb->ip_summed = CHECKSUM_NONE;
		skb_set_network_header(skb, skb->head-skb->data);
		ppp_input(&po->chan, skb);

		return NET_RX_SUCCESS;
	}
drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}