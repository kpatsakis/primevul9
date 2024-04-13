static int pptp_xmit(struct ppp_channel *chan, struct sk_buff *skb)
{
	struct sock *sk = (struct sock *) chan->private;
	struct pppox_sock *po = pppox_sk(sk);
	struct net *net = sock_net(sk);
	struct pptp_opt *opt = &po->proto.pptp;
	struct pptp_gre_header *hdr;
	unsigned int header_len = sizeof(*hdr);
	struct flowi4 fl4;
	int islcp;
	int len;
	unsigned char *data;
	__u32 seq_recv;


	struct rtable *rt;
	struct net_device *tdev;
	struct iphdr  *iph;
	int    max_headroom;

	if (sk_pppox(po)->sk_state & PPPOX_DEAD)
		goto tx_error;

	rt = ip_route_output_ports(net, &fl4, NULL,
				   opt->dst_addr.sin_addr.s_addr,
				   opt->src_addr.sin_addr.s_addr,
				   0, 0, IPPROTO_GRE,
				   RT_TOS(0), 0);
	if (IS_ERR(rt))
		goto tx_error;

	tdev = rt->dst.dev;

	max_headroom = LL_RESERVED_SPACE(tdev) + sizeof(*iph) + sizeof(*hdr) + 2;

	if (skb_headroom(skb) < max_headroom || skb_cloned(skb) || skb_shared(skb)) {
		struct sk_buff *new_skb = skb_realloc_headroom(skb, max_headroom);
		if (!new_skb) {
			ip_rt_put(rt);
			goto tx_error;
		}
		if (skb->sk)
			skb_set_owner_w(new_skb, skb->sk);
		consume_skb(skb);
		skb = new_skb;
	}

	data = skb->data;
	islcp = ((data[0] << 8) + data[1]) == PPP_LCP && 1 <= data[2] && data[2] <= 7;

	/* compress protocol field */
	if ((opt->ppp_flags & SC_COMP_PROT) && data[0] == 0 && !islcp)
		skb_pull(skb, 1);

	/* Put in the address/control bytes if necessary */
	if ((opt->ppp_flags & SC_COMP_AC) == 0 || islcp) {
		data = skb_push(skb, 2);
		data[0] = PPP_ALLSTATIONS;
		data[1] = PPP_UI;
	}

	len = skb->len;

	seq_recv = opt->seq_recv;

	if (opt->ack_sent == seq_recv)
		header_len -= sizeof(hdr->ack);

	/* Push down and install GRE header */
	skb_push(skb, header_len);
	hdr = (struct pptp_gre_header *)(skb->data);

	hdr->flags       = PPTP_GRE_FLAG_K;
	hdr->ver         = PPTP_GRE_VER;
	hdr->protocol    = htons(PPTP_GRE_PROTO);
	hdr->call_id     = htons(opt->dst_addr.call_id);

	hdr->flags      |= PPTP_GRE_FLAG_S;
	hdr->seq         = htonl(++opt->seq_sent);
	if (opt->ack_sent != seq_recv)	{
		/* send ack with this message */
		hdr->ver |= PPTP_GRE_FLAG_A;
		hdr->ack  = htonl(seq_recv);
		opt->ack_sent = seq_recv;
	}
	hdr->payload_len = htons(len);

	/*	Push down and install the IP header. */

	skb_reset_transport_header(skb);
	skb_push(skb, sizeof(*iph));
	skb_reset_network_header(skb);
	memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));
	IPCB(skb)->flags &= ~(IPSKB_XFRM_TUNNEL_SIZE | IPSKB_XFRM_TRANSFORMED | IPSKB_REROUTED);

	iph =	ip_hdr(skb);
	iph->version =	4;
	iph->ihl =	sizeof(struct iphdr) >> 2;
	if (ip_dont_fragment(sk, &rt->dst))
		iph->frag_off	=	htons(IP_DF);
	else
		iph->frag_off	=	0;
	iph->protocol = IPPROTO_GRE;
	iph->tos      = 0;
	iph->daddr    = fl4.daddr;
	iph->saddr    = fl4.saddr;
	iph->ttl      = ip4_dst_hoplimit(&rt->dst);
	iph->tot_len  = htons(skb->len);

	skb_dst_drop(skb);
	skb_dst_set(skb, &rt->dst);

	nf_reset(skb);

	skb->ip_summed = CHECKSUM_NONE;
	ip_select_ident(net, skb, NULL);
	ip_send_check(iph);

	ip_local_out(net, skb->sk, skb);
	return 1;

tx_error:
	kfree_skb(skb);
	return 1;
}