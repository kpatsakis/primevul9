static void wmi_evt_eapol_rx(struct wil6210_priv *wil, int id,
			     void *d, int len)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct wmi_eapol_rx_event *evt = d;
	u16 eapol_len = le16_to_cpu(evt->eapol_len);
	int sz = eapol_len + ETH_HLEN;
	struct sk_buff *skb;
	struct ethhdr *eth;
	int cid;
	struct wil_net_stats *stats = NULL;

	wil_dbg_wmi(wil, "EAPOL len %d from %pM\n", eapol_len,
		    evt->src_mac);

	cid = wil_find_cid(wil, evt->src_mac);
	if (cid >= 0)
		stats = &wil->sta[cid].stats;

	if (eapol_len > 196) { /* TODO: revisit size limit */
		wil_err(wil, "EAPOL too large\n");
		return;
	}

	skb = alloc_skb(sz, GFP_KERNEL);
	if (!skb) {
		wil_err(wil, "Failed to allocate skb\n");
		return;
	}

	eth = skb_put(skb, ETH_HLEN);
	ether_addr_copy(eth->h_dest, ndev->dev_addr);
	ether_addr_copy(eth->h_source, evt->src_mac);
	eth->h_proto = cpu_to_be16(ETH_P_PAE);
	skb_put_data(skb, evt->eapol, eapol_len);
	skb->protocol = eth_type_trans(skb, ndev);
	if (likely(netif_rx_ni(skb) == NET_RX_SUCCESS)) {
		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += sz;
		if (stats) {
			stats->rx_packets++;
			stats->rx_bytes += sz;
		}
	} else {
		ndev->stats.rx_dropped++;
		if (stats)
			stats->rx_dropped++;
	}
}