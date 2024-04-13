isdn_net_log_skb(struct sk_buff *skb, isdn_net_local *lp)
{
	/* hopefully, this was set correctly */
	const u_char *p = skb_network_header(skb);
	unsigned short proto = ntohs(skb->protocol);
	int data_ofs;
	ip_ports *ipp;
	char addinfo[100];

	addinfo[0] = '\0';
	/* This check stolen from 2.1.72 dev_queue_xmit_nit() */
	if (p < skb->data || skb_network_header(skb) >= skb_tail_pointer(skb)) {
		/* fall back to old isdn_net_log_packet method() */
		char *buf = skb->data;

		printk(KERN_DEBUG "isdn_net: protocol %04x is buggy, dev %s\n", skb->protocol, lp->netdev->dev->name);
		p = buf;
		proto = ETH_P_IP;
		switch (lp->p_encap) {
		case ISDN_NET_ENCAP_IPTYP:
			proto = ntohs(*(__be16 *)&buf[0]);
			p = &buf[2];
			break;
		case ISDN_NET_ENCAP_ETHER:
			proto = ntohs(*(__be16 *)&buf[12]);
			p = &buf[14];
			break;
		case ISDN_NET_ENCAP_CISCOHDLC:
			proto = ntohs(*(__be16 *)&buf[2]);
			p = &buf[4];
			break;
#ifdef CONFIG_ISDN_PPP
		case ISDN_NET_ENCAP_SYNCPPP:
			proto = ntohs(skb->protocol);
			p = &buf[IPPP_MAX_HEADER];
			break;
#endif
		}
	}
	data_ofs = ((p[0] & 15) * 4);
	switch (proto) {
	case ETH_P_IP:
		switch (p[9]) {
		case 1:
			strcpy(addinfo, " ICMP");
			break;
		case 2:
			strcpy(addinfo, " IGMP");
			break;
		case 4:
			strcpy(addinfo, " IPIP");
			break;
		case 6:
			ipp = (ip_ports *) (&p[data_ofs]);
			sprintf(addinfo, " TCP, port: %d -> %d", ntohs(ipp->source),
				ntohs(ipp->dest));
			break;
		case 8:
			strcpy(addinfo, " EGP");
			break;
		case 12:
			strcpy(addinfo, " PUP");
			break;
		case 17:
			ipp = (ip_ports *) (&p[data_ofs]);
			sprintf(addinfo, " UDP, port: %d -> %d", ntohs(ipp->source),
				ntohs(ipp->dest));
			break;
		case 22:
			strcpy(addinfo, " IDP");
			break;
		}
		printk(KERN_INFO "OPEN: %pI4 -> %pI4%s\n",
		       p + 12, p + 16, addinfo);
		break;
	case ETH_P_ARP:
		printk(KERN_INFO "OPEN: ARP %pI4 -> *.*.*.* ?%pI4\n",
		       p + 14, p + 24);
		break;
	}
}