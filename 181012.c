u8 ath6kl_wmi_determine_user_priority(u8 *pkt, u32 layer2_pri)
{
	struct iphdr *ip_hdr = (struct iphdr *) pkt;
	u8 ip_pri;

	/*
	 * Determine IPTOS priority
	 *
	 * IP-TOS - 8bits
	 *          : DSCP(6-bits) ECN(2-bits)
	 *          : DSCP - P2 P1 P0 X X X
	 * where (P2 P1 P0) form 802.1D
	 */
	ip_pri = ip_hdr->tos >> 5;
	ip_pri &= 0x7;

	if ((layer2_pri & 0x7) > ip_pri)
		return (u8) layer2_pri & 0x7;
	else
		return ip_pri;
}