ieee80211_rx_frame_decrypt_msdu(struct ieee80211_device *ieee,
				struct sk_buff *skb, int keyidx,
				struct ieee80211_crypt_data *crypt)
{
	struct ieee80211_hdr_3addr *hdr;
	int res, hdrlen;

	if (crypt == NULL || crypt->ops->decrypt_msdu == NULL)
		return 0;

	hdr = (struct ieee80211_hdr_3addr *)skb->data;
	hdrlen = ieee80211_get_hdrlen(le16_to_cpu(hdr->frame_ctl));

	atomic_inc(&crypt->refcnt);
	res = crypt->ops->decrypt_msdu(skb, keyidx, hdrlen, crypt->priv);
	atomic_dec(&crypt->refcnt);
	if (res < 0) {
		printk(KERN_DEBUG "%s: MSDU decryption/MIC verification failed"
		       " (SA=" MAC_FMT " keyidx=%d)\n",
		       ieee->dev->name, MAC_ARG(hdr->addr2), keyidx);
		return -1;
	}

	return 0;
}