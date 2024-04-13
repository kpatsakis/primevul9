int ath6kl_wmi_dot11_hdr_remove(struct wmi *wmi, struct sk_buff *skb)
{
	struct ieee80211_hdr_3addr *pwh, wh;
	struct ath6kl_llc_snap_hdr *llc_hdr;
	struct ethhdr eth_hdr;
	u32 hdr_size;
	u8 *datap;
	__le16 sub_type;

	if (WARN_ON(skb == NULL))
		return -EINVAL;

	datap = skb->data;
	pwh = (struct ieee80211_hdr_3addr *) datap;

	sub_type = pwh->frame_control & cpu_to_le16(IEEE80211_FCTL_STYPE);

	memcpy((u8 *) &wh, datap, sizeof(struct ieee80211_hdr_3addr));

	/* Strip off the 802.11 header */
	if (sub_type == cpu_to_le16(IEEE80211_STYPE_QOS_DATA)) {
		hdr_size = roundup(sizeof(struct ieee80211_qos_hdr),
				   sizeof(u32));
		skb_pull(skb, hdr_size);
	} else if (sub_type == cpu_to_le16(IEEE80211_STYPE_DATA)) {
		skb_pull(skb, sizeof(struct ieee80211_hdr_3addr));
	}

	datap = skb->data;
	llc_hdr = (struct ath6kl_llc_snap_hdr *)(datap);

	memset(&eth_hdr, 0, sizeof(eth_hdr));
	eth_hdr.h_proto = llc_hdr->eth_type;

	switch ((le16_to_cpu(wh.frame_control)) &
		(IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS)) {
	case IEEE80211_FCTL_TODS:
		memcpy(eth_hdr.h_dest, wh.addr3, ETH_ALEN);
		memcpy(eth_hdr.h_source, wh.addr2, ETH_ALEN);
		break;
	case IEEE80211_FCTL_FROMDS:
		memcpy(eth_hdr.h_dest, wh.addr1, ETH_ALEN);
		memcpy(eth_hdr.h_source, wh.addr3, ETH_ALEN);
		break;
	case IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS:
		break;
	default:
		memcpy(eth_hdr.h_dest, wh.addr1, ETH_ALEN);
		memcpy(eth_hdr.h_source, wh.addr2, ETH_ALEN);
		break;
	}

	skb_pull(skb, sizeof(struct ath6kl_llc_snap_hdr));
	skb_push(skb, sizeof(eth_hdr));

	datap = skb->data;

	memcpy(datap, &eth_hdr, sizeof(eth_hdr));

	return 0;
}