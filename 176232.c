ax88179_rx_checksum(struct sk_buff *skb, u32 *pkt_hdr)
{
	skb->ip_summed = CHECKSUM_NONE;

	/* checksum error bit is set */
	if ((*pkt_hdr & AX_RXHDR_L3CSUM_ERR) ||
	    (*pkt_hdr & AX_RXHDR_L4CSUM_ERR))
		return;

	/* It must be a TCP or UDP packet with a valid checksum */
	if (((*pkt_hdr & AX_RXHDR_L4_TYPE_MASK) == AX_RXHDR_L4_TYPE_TCP) ||
	    ((*pkt_hdr & AX_RXHDR_L4_TYPE_MASK) == AX_RXHDR_L4_TYPE_UDP))
		skb->ip_summed = CHECKSUM_UNNECESSARY;
}