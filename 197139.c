static u32 bond_vlan_srcmac_hash(struct sk_buff *skb)
{
	struct ethhdr *mac_hdr = (struct ethhdr *)skb_mac_header(skb);
	u32 srcmac_vendor = 0, srcmac_dev = 0;
	u16 vlan;
	int i;

	for (i = 0; i < 3; i++)
		srcmac_vendor = (srcmac_vendor << 8) | mac_hdr->h_source[i];

	for (i = 3; i < ETH_ALEN; i++)
		srcmac_dev = (srcmac_dev << 8) | mac_hdr->h_source[i];

	if (!skb_vlan_tag_present(skb))
		return srcmac_vendor ^ srcmac_dev;

	vlan = skb_vlan_tag_get(skb);

	return vlan ^ srcmac_vendor ^ srcmac_dev;
}