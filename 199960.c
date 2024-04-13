ieee80211_rx_frame_decrypt(struct ieee80211_device *ieee, struct sk_buff *skb,
			   struct ieee80211_crypt_data *crypt)
{
	struct ieee80211_hdr_3addr *hdr;
	int res, hdrlen;

	if (crypt == NULL || crypt->ops->decrypt_mpdu == NULL)
		return 0;

	hdr = (struct ieee80211_hdr_3addr *)skb->data;
	hdrlen = ieee80211_get_hdrlen(le16_to_cpu(hdr->frame_ctl));

	atomic_inc(&crypt->refcnt);
	res = crypt->ops->decrypt_mpdu(skb, hdrlen, crypt->priv);
	atomic_dec(&crypt->refcnt);
	if (res < 0) {
		IEEE80211_DEBUG_DROP("decryption failed (SA=" MAC_FMT
				     ") res=%d\n", MAC_ARG(hdr->addr2), res);
		if (res == -2)
			IEEE80211_DEBUG_DROP("Decryption failed ICV "
					     "mismatch (key %d)\n",
					     skb->data[hdrlen + 3] >> 6);
		ieee->ieee_stats.rx_discards_undecryptable++;
		return -1;
	}

	return res;
}