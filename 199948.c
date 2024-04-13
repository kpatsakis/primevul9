static int ieee80211_is_eapol_frame(struct ieee80211_device *ieee,
				    struct sk_buff *skb)
{
	struct net_device *dev = ieee->dev;
	u16 fc, ethertype;
	struct ieee80211_hdr_3addr *hdr;
	u8 *pos;

	if (skb->len < 24)
		return 0;

	hdr = (struct ieee80211_hdr_3addr *)skb->data;
	fc = le16_to_cpu(hdr->frame_ctl);

	/* check that the frame is unicast frame to us */
	if ((fc & (IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS)) ==
	    IEEE80211_FCTL_TODS &&
	    !compare_ether_addr(hdr->addr1, dev->dev_addr) &&
	    !compare_ether_addr(hdr->addr3, dev->dev_addr)) {
		/* ToDS frame with own addr BSSID and DA */
	} else if ((fc & (IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS)) ==
		   IEEE80211_FCTL_FROMDS &&
		   !compare_ether_addr(hdr->addr1, dev->dev_addr)) {
		/* FromDS frame with own addr as DA */
	} else
		return 0;

	if (skb->len < 24 + 8)
		return 0;

	/* check for port access entity Ethernet type */
	pos = skb->data + 24;
	ethertype = (pos[6] << 8) | pos[7];
	if (ethertype == ETH_P_PAE)
		return 1;

	return 0;
}