static int __ovs_nla_put_key(const struct sw_flow_key *swkey,
			     const struct sw_flow_key *output, bool is_mask,
			     struct sk_buff *skb)
{
	struct ovs_key_ethernet *eth_key;
	struct nlattr *nla;
	struct nlattr *encap = NULL;
	struct nlattr *in_encap = NULL;

	if (nla_put_u32(skb, OVS_KEY_ATTR_RECIRC_ID, output->recirc_id))
		goto nla_put_failure;

	if (nla_put_u32(skb, OVS_KEY_ATTR_DP_HASH, output->ovs_flow_hash))
		goto nla_put_failure;

	if (nla_put_u32(skb, OVS_KEY_ATTR_PRIORITY, output->phy.priority))
		goto nla_put_failure;

	if ((swkey->tun_proto || is_mask)) {
		const void *opts = NULL;

		if (output->tun_key.tun_flags & TUNNEL_OPTIONS_PRESENT)
			opts = TUN_METADATA_OPTS(output, swkey->tun_opts_len);

		if (ip_tun_to_nlattr(skb, &output->tun_key, opts,
				     swkey->tun_opts_len, swkey->tun_proto, 0))
			goto nla_put_failure;
	}

	if (swkey->phy.in_port == DP_MAX_PORTS) {
		if (is_mask && (output->phy.in_port == 0xffff))
			if (nla_put_u32(skb, OVS_KEY_ATTR_IN_PORT, 0xffffffff))
				goto nla_put_failure;
	} else {
		u16 upper_u16;
		upper_u16 = !is_mask ? 0 : 0xffff;

		if (nla_put_u32(skb, OVS_KEY_ATTR_IN_PORT,
				(upper_u16 << 16) | output->phy.in_port))
			goto nla_put_failure;
	}

	if (nla_put_u32(skb, OVS_KEY_ATTR_SKB_MARK, output->phy.skb_mark))
		goto nla_put_failure;

	if (ovs_ct_put_key(swkey, output, skb))
		goto nla_put_failure;

	if (ovs_key_mac_proto(swkey) == MAC_PROTO_ETHERNET) {
		nla = nla_reserve(skb, OVS_KEY_ATTR_ETHERNET, sizeof(*eth_key));
		if (!nla)
			goto nla_put_failure;

		eth_key = nla_data(nla);
		ether_addr_copy(eth_key->eth_src, output->eth.src);
		ether_addr_copy(eth_key->eth_dst, output->eth.dst);

		if (swkey->eth.vlan.tci || eth_type_vlan(swkey->eth.type)) {
			if (ovs_nla_put_vlan(skb, &output->eth.vlan, is_mask))
				goto nla_put_failure;
			encap = nla_nest_start_noflag(skb, OVS_KEY_ATTR_ENCAP);
			if (!swkey->eth.vlan.tci)
				goto unencap;

			if (swkey->eth.cvlan.tci || eth_type_vlan(swkey->eth.type)) {
				if (ovs_nla_put_vlan(skb, &output->eth.cvlan, is_mask))
					goto nla_put_failure;
				in_encap = nla_nest_start_noflag(skb,
								 OVS_KEY_ATTR_ENCAP);
				if (!swkey->eth.cvlan.tci)
					goto unencap;
			}
		}

		if (swkey->eth.type == htons(ETH_P_802_2)) {
			/*
			* Ethertype 802.2 is represented in the netlink with omitted
			* OVS_KEY_ATTR_ETHERTYPE in the flow key attribute, and
			* 0xffff in the mask attribute.  Ethertype can also
			* be wildcarded.
			*/
			if (is_mask && output->eth.type)
				if (nla_put_be16(skb, OVS_KEY_ATTR_ETHERTYPE,
							output->eth.type))
					goto nla_put_failure;
			goto unencap;
		}
	}

	if (nla_put_be16(skb, OVS_KEY_ATTR_ETHERTYPE, output->eth.type))
		goto nla_put_failure;

	if (eth_type_vlan(swkey->eth.type)) {
		/* There are 3 VLAN tags, we don't know anything about the rest
		 * of the packet, so truncate here.
		 */
		WARN_ON_ONCE(!(encap && in_encap));
		goto unencap;
	}

	if (swkey->eth.type == htons(ETH_P_IP)) {
		struct ovs_key_ipv4 *ipv4_key;

		nla = nla_reserve(skb, OVS_KEY_ATTR_IPV4, sizeof(*ipv4_key));
		if (!nla)
			goto nla_put_failure;
		ipv4_key = nla_data(nla);
		ipv4_key->ipv4_src = output->ipv4.addr.src;
		ipv4_key->ipv4_dst = output->ipv4.addr.dst;
		ipv4_key->ipv4_proto = output->ip.proto;
		ipv4_key->ipv4_tos = output->ip.tos;
		ipv4_key->ipv4_ttl = output->ip.ttl;
		ipv4_key->ipv4_frag = output->ip.frag;
	} else if (swkey->eth.type == htons(ETH_P_IPV6)) {
		struct ovs_key_ipv6 *ipv6_key;
		struct ovs_key_ipv6_exthdrs *ipv6_exthdrs_key;

		nla = nla_reserve(skb, OVS_KEY_ATTR_IPV6, sizeof(*ipv6_key));
		if (!nla)
			goto nla_put_failure;
		ipv6_key = nla_data(nla);
		memcpy(ipv6_key->ipv6_src, &output->ipv6.addr.src,
				sizeof(ipv6_key->ipv6_src));
		memcpy(ipv6_key->ipv6_dst, &output->ipv6.addr.dst,
				sizeof(ipv6_key->ipv6_dst));
		ipv6_key->ipv6_label = output->ipv6.label;
		ipv6_key->ipv6_proto = output->ip.proto;
		ipv6_key->ipv6_tclass = output->ip.tos;
		ipv6_key->ipv6_hlimit = output->ip.ttl;
		ipv6_key->ipv6_frag = output->ip.frag;

		nla = nla_reserve(skb, OVS_KEY_ATTR_IPV6_EXTHDRS,
				  sizeof(*ipv6_exthdrs_key));
		if (!nla)
			goto nla_put_failure;
		ipv6_exthdrs_key = nla_data(nla);
		ipv6_exthdrs_key->hdrs = output->ipv6.exthdrs;
	} else if (swkey->eth.type == htons(ETH_P_NSH)) {
		if (nsh_key_to_nlattr(&output->nsh, is_mask, skb))
			goto nla_put_failure;
	} else if (swkey->eth.type == htons(ETH_P_ARP) ||
		   swkey->eth.type == htons(ETH_P_RARP)) {
		struct ovs_key_arp *arp_key;

		nla = nla_reserve(skb, OVS_KEY_ATTR_ARP, sizeof(*arp_key));
		if (!nla)
			goto nla_put_failure;
		arp_key = nla_data(nla);
		memset(arp_key, 0, sizeof(struct ovs_key_arp));
		arp_key->arp_sip = output->ipv4.addr.src;
		arp_key->arp_tip = output->ipv4.addr.dst;
		arp_key->arp_op = htons(output->ip.proto);
		ether_addr_copy(arp_key->arp_sha, output->ipv4.arp.sha);
		ether_addr_copy(arp_key->arp_tha, output->ipv4.arp.tha);
	} else if (eth_p_mpls(swkey->eth.type)) {
		u8 i, num_labels;
		struct ovs_key_mpls *mpls_key;

		num_labels = hweight_long(output->mpls.num_labels_mask);
		nla = nla_reserve(skb, OVS_KEY_ATTR_MPLS,
				  num_labels * sizeof(*mpls_key));
		if (!nla)
			goto nla_put_failure;

		mpls_key = nla_data(nla);
		for (i = 0; i < num_labels; i++)
			mpls_key[i].mpls_lse = output->mpls.lse[i];
	}

	if ((swkey->eth.type == htons(ETH_P_IP) ||
	     swkey->eth.type == htons(ETH_P_IPV6)) &&
	     swkey->ip.frag != OVS_FRAG_TYPE_LATER) {

		if (swkey->ip.proto == IPPROTO_TCP) {
			struct ovs_key_tcp *tcp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_TCP, sizeof(*tcp_key));
			if (!nla)
				goto nla_put_failure;
			tcp_key = nla_data(nla);
			tcp_key->tcp_src = output->tp.src;
			tcp_key->tcp_dst = output->tp.dst;
			if (nla_put_be16(skb, OVS_KEY_ATTR_TCP_FLAGS,
					 output->tp.flags))
				goto nla_put_failure;
		} else if (swkey->ip.proto == IPPROTO_UDP) {
			struct ovs_key_udp *udp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_UDP, sizeof(*udp_key));
			if (!nla)
				goto nla_put_failure;
			udp_key = nla_data(nla);
			udp_key->udp_src = output->tp.src;
			udp_key->udp_dst = output->tp.dst;
		} else if (swkey->ip.proto == IPPROTO_SCTP) {
			struct ovs_key_sctp *sctp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_SCTP, sizeof(*sctp_key));
			if (!nla)
				goto nla_put_failure;
			sctp_key = nla_data(nla);
			sctp_key->sctp_src = output->tp.src;
			sctp_key->sctp_dst = output->tp.dst;
		} else if (swkey->eth.type == htons(ETH_P_IP) &&
			   swkey->ip.proto == IPPROTO_ICMP) {
			struct ovs_key_icmp *icmp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_ICMP, sizeof(*icmp_key));
			if (!nla)
				goto nla_put_failure;
			icmp_key = nla_data(nla);
			icmp_key->icmp_type = ntohs(output->tp.src);
			icmp_key->icmp_code = ntohs(output->tp.dst);
		} else if (swkey->eth.type == htons(ETH_P_IPV6) &&
			   swkey->ip.proto == IPPROTO_ICMPV6) {
			struct ovs_key_icmpv6 *icmpv6_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_ICMPV6,
						sizeof(*icmpv6_key));
			if (!nla)
				goto nla_put_failure;
			icmpv6_key = nla_data(nla);
			icmpv6_key->icmpv6_type = ntohs(output->tp.src);
			icmpv6_key->icmpv6_code = ntohs(output->tp.dst);

			if (swkey->tp.src == htons(NDISC_NEIGHBOUR_SOLICITATION) ||
			    swkey->tp.src == htons(NDISC_NEIGHBOUR_ADVERTISEMENT)) {
				struct ovs_key_nd *nd_key;

				nla = nla_reserve(skb, OVS_KEY_ATTR_ND, sizeof(*nd_key));
				if (!nla)
					goto nla_put_failure;
				nd_key = nla_data(nla);
				memcpy(nd_key->nd_target, &output->ipv6.nd.target,
							sizeof(nd_key->nd_target));
				ether_addr_copy(nd_key->nd_sll, output->ipv6.nd.sll);
				ether_addr_copy(nd_key->nd_tll, output->ipv6.nd.tll);
			}
		}
	}

unencap:
	if (in_encap)
		nla_nest_end(skb, in_encap);
	if (encap)
		nla_nest_end(skb, encap);

	return 0;

nla_put_failure:
	return -EMSGSIZE;
}