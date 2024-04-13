static void ieee80211_monitor_rx(struct ieee80211_device *ieee,
					struct sk_buff *skb,
					struct ieee80211_rx_stats *rx_stats)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	u16 fc = le16_to_cpu(hdr->frame_ctl);

	skb->dev = ieee->dev;
	skb_reset_mac_header(skb);
	skb_pull(skb, ieee80211_get_hdrlen(fc));
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = __constant_htons(ETH_P_80211_RAW);
	memset(skb->cb, 0, sizeof(skb->cb));
	netif_rx(skb);
}