decode_session6(struct sk_buff *skb, struct flowi *fl, bool reverse)
{
	struct flowi6 *fl6 = &fl->u.ip6;
	int onlyproto = 0;
	const struct ipv6hdr *hdr = ipv6_hdr(skb);
	u32 offset = sizeof(*hdr);
	struct ipv6_opt_hdr *exthdr;
	const unsigned char *nh = skb_network_header(skb);
	u16 nhoff = IP6CB(skb)->nhoff;
	int oif = 0;
	u8 nexthdr;

	if (!nhoff)
		nhoff = offsetof(struct ipv6hdr, nexthdr);

	nexthdr = nh[nhoff];

	if (skb_dst(skb) && skb_dst(skb)->dev)
		oif = skb_dst(skb)->dev->ifindex;

	memset(fl6, 0, sizeof(struct flowi6));
	fl6->flowi6_mark = skb->mark;
	fl6->flowi6_oif = reverse ? skb->skb_iif : oif;

	fl6->daddr = reverse ? hdr->saddr : hdr->daddr;
	fl6->saddr = reverse ? hdr->daddr : hdr->saddr;

	while (nh + offset + sizeof(*exthdr) < skb->data ||
	       pskb_may_pull(skb, nh + offset + sizeof(*exthdr) - skb->data)) {
		nh = skb_network_header(skb);
		exthdr = (struct ipv6_opt_hdr *)(nh + offset);

		switch (nexthdr) {
		case NEXTHDR_FRAGMENT:
			onlyproto = 1;
			fallthrough;
		case NEXTHDR_ROUTING:
		case NEXTHDR_HOP:
		case NEXTHDR_DEST:
			offset += ipv6_optlen(exthdr);
			nexthdr = exthdr->nexthdr;
			break;
		case IPPROTO_UDP:
		case IPPROTO_UDPLITE:
		case IPPROTO_TCP:
		case IPPROTO_SCTP:
		case IPPROTO_DCCP:
			if (!onlyproto && (nh + offset + 4 < skb->data ||
			     pskb_may_pull(skb, nh + offset + 4 - skb->data))) {
				__be16 *ports;

				nh = skb_network_header(skb);
				ports = (__be16 *)(nh + offset);
				fl6->fl6_sport = ports[!!reverse];
				fl6->fl6_dport = ports[!reverse];
			}
			fl6->flowi6_proto = nexthdr;
			return;
		case IPPROTO_ICMPV6:
			if (!onlyproto && (nh + offset + 2 < skb->data ||
			    pskb_may_pull(skb, nh + offset + 2 - skb->data))) {
				u8 *icmp;

				nh = skb_network_header(skb);
				icmp = (u8 *)(nh + offset);
				fl6->fl6_icmp_type = icmp[0];
				fl6->fl6_icmp_code = icmp[1];
			}
			fl6->flowi6_proto = nexthdr;
			return;
		case IPPROTO_GRE:
			if (!onlyproto &&
			    (nh + offset + 12 < skb->data ||
			     pskb_may_pull(skb, nh + offset + 12 - skb->data))) {
				struct gre_base_hdr *gre_hdr;
				__be32 *gre_key;

				nh = skb_network_header(skb);
				gre_hdr = (struct gre_base_hdr *)(nh + offset);
				gre_key = (__be32 *)(gre_hdr + 1);

				if (gre_hdr->flags & GRE_KEY) {
					if (gre_hdr->flags & GRE_CSUM)
						gre_key++;
					fl6->fl6_gre_key = *gre_key;
				}
			}
			fl6->flowi6_proto = nexthdr;
			return;

#if IS_ENABLED(CONFIG_IPV6_MIP6)
		case IPPROTO_MH:
			offset += ipv6_optlen(exthdr);
			if (!onlyproto && (nh + offset + 3 < skb->data ||
			    pskb_may_pull(skb, nh + offset + 3 - skb->data))) {
				struct ip6_mh *mh;

				nh = skb_network_header(skb);
				mh = (struct ip6_mh *)(nh + offset);
				fl6->fl6_mh_type = mh->ip6mh_type;
			}
			fl6->flowi6_proto = nexthdr;
			return;
#endif
		default:
			fl6->flowi6_proto = nexthdr;
			return;
		}
	}
}