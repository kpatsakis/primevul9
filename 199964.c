static int ieee80211_frag_cache_invalidate(struct ieee80211_device *ieee,
					   struct ieee80211_hdr_4addr *hdr)
{
	u16 sc;
	unsigned int seq;
	struct ieee80211_frag_entry *entry;

	sc = le16_to_cpu(hdr->seq_ctl);
	seq = WLAN_GET_SEQ_SEQ(sc);

	entry = ieee80211_frag_cache_find(ieee, seq, -1, hdr->addr2,
					  hdr->addr1);

	if (entry == NULL) {
		IEEE80211_DEBUG_FRAG("could not invalidate fragment cache "
				     "entry (seq=%u)\n", seq);
		return -1;
	}

	entry->skb = NULL;
	return 0;
}