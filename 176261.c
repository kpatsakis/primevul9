ax88179_tx_fixup(struct usbnet *dev, struct sk_buff *skb, gfp_t flags)
{
	u32 tx_hdr1, tx_hdr2;
	int frame_size = dev->maxpacket;
	int headroom;
	void *ptr;

	tx_hdr1 = skb->len;
	tx_hdr2 = skb_shinfo(skb)->gso_size; /* Set TSO mss */
	if (((skb->len + 8) % frame_size) == 0)
		tx_hdr2 |= 0x80008000;	/* Enable padding */

	headroom = skb_headroom(skb) - 8;

	if ((dev->net->features & NETIF_F_SG) && skb_linearize(skb))
		return NULL;

	if ((skb_header_cloned(skb) || headroom < 0) &&
	    pskb_expand_head(skb, headroom < 0 ? 8 : 0, 0, GFP_ATOMIC)) {
		dev_kfree_skb_any(skb);
		return NULL;
	}

	ptr = skb_push(skb, 8);
	put_unaligned_le32(tx_hdr1, ptr);
	put_unaligned_le32(tx_hdr2, ptr + 4);

	usbnet_set_skb_tx_stats(skb, (skb_shinfo(skb)->gso_segs ?: 1), 0);

	return skb;
}