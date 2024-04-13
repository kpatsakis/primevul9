int ath6kl_wmi_dix_2_dot3(struct wmi *wmi, struct sk_buff *skb)
{
	struct ath6kl_llc_snap_hdr *llc_hdr;
	struct ethhdr *eth_hdr;
	size_t new_len;
	__be16 type;
	u8 *datap;
	u16 size;

	if (WARN_ON(skb == NULL))
		return -EINVAL;

	size = sizeof(struct ath6kl_llc_snap_hdr) + sizeof(struct wmi_data_hdr);
	if (skb_headroom(skb) < size)
		return -ENOMEM;

	eth_hdr = (struct ethhdr *) skb->data;
	type = eth_hdr->h_proto;

	if (!is_ethertype(be16_to_cpu(type))) {
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "%s: pkt is already in 802.3 format\n", __func__);
		return 0;
	}

	new_len = skb->len - sizeof(*eth_hdr) + sizeof(*llc_hdr);

	skb_push(skb, sizeof(struct ath6kl_llc_snap_hdr));
	datap = skb->data;

	eth_hdr->h_proto = cpu_to_be16(new_len);

	memcpy(datap, eth_hdr, sizeof(*eth_hdr));

	llc_hdr = (struct ath6kl_llc_snap_hdr *)(datap + sizeof(*eth_hdr));
	llc_hdr->dsap = 0xAA;
	llc_hdr->ssap = 0xAA;
	llc_hdr->cntl = 0x03;
	llc_hdr->org_code[0] = 0x0;
	llc_hdr->org_code[1] = 0x0;
	llc_hdr->org_code[2] = 0x0;
	llc_hdr->eth_type = type;

	return 0;
}