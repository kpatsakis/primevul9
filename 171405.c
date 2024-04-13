
static void ql_hw_csum_setup(const struct sk_buff *skb,
			     struct ob_mac_iocb_req *mac_iocb_ptr)
{
	const struct iphdr *ip = ip_hdr(skb);

	mac_iocb_ptr->ip_hdr_off = skb_network_offset(skb);
	mac_iocb_ptr->ip_hdr_len = ip->ihl;

	if (ip->protocol == IPPROTO_TCP) {
		mac_iocb_ptr->flags1 |= OB_3032MAC_IOCB_REQ_TC |
			OB_3032MAC_IOCB_REQ_IC;
	} else {
		mac_iocb_ptr->flags1 |= OB_3032MAC_IOCB_REQ_UC |
			OB_3032MAC_IOCB_REQ_IC;
	}
