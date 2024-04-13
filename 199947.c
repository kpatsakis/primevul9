static struct ieee80211_frag_entry *ieee80211_frag_cache_find(struct
							      ieee80211_device
							      *ieee,
							      unsigned int seq,
							      unsigned int frag,
							      u8 * src,
							      u8 * dst)
{
	struct ieee80211_frag_entry *entry;
	int i;

	for (i = 0; i < IEEE80211_FRAG_CACHE_LEN; i++) {
		entry = &ieee->frag_cache[i];
		if (entry->skb != NULL &&
		    time_after(jiffies, entry->first_frag_time + 2 * HZ)) {
			IEEE80211_DEBUG_FRAG("expiring fragment cache entry "
					     "seq=%u last_frag=%u\n",
					     entry->seq, entry->last_frag);
			dev_kfree_skb_any(entry->skb);
			entry->skb = NULL;
		}

		if (entry->skb != NULL && entry->seq == seq &&
		    (entry->last_frag + 1 == frag || frag == -1) &&
		    !compare_ether_addr(entry->src_addr, src) &&
		    !compare_ether_addr(entry->dst_addr, dst))
			return entry;
	}

	return NULL;
}