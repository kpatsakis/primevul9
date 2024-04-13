void ieee80211_rx_any(struct ieee80211_device *ieee,
		     struct sk_buff *skb, struct ieee80211_rx_stats *stats)
{
	struct ieee80211_hdr_4addr *hdr;
	int is_packet_for_us;
	u16 fc;

	if (ieee->iw_mode == IW_MODE_MONITOR) {
		if (!ieee80211_rx(ieee, skb, stats))
			dev_kfree_skb_irq(skb);
		return;
	}

	if (skb->len < sizeof(struct ieee80211_hdr))
		goto drop_free;

	hdr = (struct ieee80211_hdr_4addr *)skb->data;
	fc = le16_to_cpu(hdr->frame_ctl);

	if ((fc & IEEE80211_FCTL_VERS) != 0)
		goto drop_free;

	switch (fc & IEEE80211_FCTL_FTYPE) {
	case IEEE80211_FTYPE_MGMT:
		if (skb->len < sizeof(struct ieee80211_hdr_3addr))
			goto drop_free;
		ieee80211_rx_mgt(ieee, hdr, stats);
		dev_kfree_skb_irq(skb);
		return;
	case IEEE80211_FTYPE_DATA:
		break;
	case IEEE80211_FTYPE_CTL:
		return;
	default:
		return;
	}

	is_packet_for_us = 0;
	switch (ieee->iw_mode) {
	case IW_MODE_ADHOC:
		/* our BSS and not from/to DS */
		if (memcmp(hdr->addr3, ieee->bssid, ETH_ALEN) == 0)
		if ((fc & (IEEE80211_FCTL_TODS+IEEE80211_FCTL_FROMDS)) == 0) {
			/* promisc: get all */
			if (ieee->dev->flags & IFF_PROMISC)
				is_packet_for_us = 1;
			/* to us */
			else if (memcmp(hdr->addr1, ieee->dev->dev_addr, ETH_ALEN) == 0)
				is_packet_for_us = 1;
			/* mcast */
			else if (is_multicast_ether_addr(hdr->addr1))
				is_packet_for_us = 1;
		}
		break;
	case IW_MODE_INFRA:
		/* our BSS (== from our AP) and from DS */
		if (memcmp(hdr->addr2, ieee->bssid, ETH_ALEN) == 0)
		if ((fc & (IEEE80211_FCTL_TODS+IEEE80211_FCTL_FROMDS)) == IEEE80211_FCTL_FROMDS) {
			/* promisc: get all */
			if (ieee->dev->flags & IFF_PROMISC)
				is_packet_for_us = 1;
			/* to us */
			else if (memcmp(hdr->addr1, ieee->dev->dev_addr, ETH_ALEN) == 0)
				is_packet_for_us = 1;
			/* mcast */
			else if (is_multicast_ether_addr(hdr->addr1)) {
				/* not our own packet bcasted from AP */
				if (memcmp(hdr->addr3, ieee->dev->dev_addr, ETH_ALEN))
					is_packet_for_us = 1;
			}
		}
		break;
	default:
		/* ? */
		break;
	}

	if (is_packet_for_us)
		if (!ieee80211_rx(ieee, skb, stats))
			dev_kfree_skb_irq(skb);
	return;

drop_free:
	dev_kfree_skb_irq(skb);
	ieee->stats.rx_dropped++;
	return;
}