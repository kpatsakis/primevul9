static struct sk_buff *ieee80211_frag_cache_get(struct ieee80211_device *ieee,
						struct ieee80211_hdr_4addr *hdr)
{
	struct sk_buff *skb = NULL;
	u16 sc;
	unsigned int frag, seq;
	struct ieee80211_frag_entry *entry;

	sc = le16_to_cpu(hdr->seq_ctl);
	frag = WLAN_GET_SEQ_FRAG(sc);
	seq = WLAN_GET_SEQ_SEQ(sc);

	if (frag == 0) {
		/* Reserve enough space to fit maximum frame length */
		skb = dev_alloc_skb(ieee->dev->mtu +
				    sizeof(struct ieee80211_hdr_4addr) +
				    8 /* LLC */  +
				    2 /* alignment */  +
				    8 /* WEP */  + ETH_ALEN /* WDS */ );
		if (skb == NULL)
			return NULL;

		entry = &ieee->frag_cache[ieee->frag_next_idx];
		ieee->frag_next_idx++;
		if (ieee->frag_next_idx >= IEEE80211_FRAG_CACHE_LEN)
			ieee->frag_next_idx = 0;

		if (entry->skb != NULL)
			dev_kfree_skb_any(entry->skb);

		entry->first_frag_time = jiffies;
		entry->seq = seq;
		entry->last_frag = frag;
		entry->skb = skb;
		memcpy(entry->src_addr, hdr->addr2, ETH_ALEN);
		memcpy(entry->dst_addr, hdr->addr1, ETH_ALEN);
	} else {
		/* received a fragment of a frame for which the head fragment
		 * should have already been received */
		entry = ieee80211_frag_cache_find(ieee, seq, frag, hdr->addr2,
						  hdr->addr1);
		if (entry != NULL) {
			entry->last_frag = frag;
			skb = entry->skb;
		}
	}

	return skb;
}