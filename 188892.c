static netdev_features_t i40e_features_check(struct sk_buff *skb,
					     struct net_device *dev,
					     netdev_features_t features)
{
	size_t len;

	/* No point in doing any of this if neither checksum nor GSO are
	 * being requested for this frame.  We can rule out both by just
	 * checking for CHECKSUM_PARTIAL
	 */
	if (skb->ip_summed != CHECKSUM_PARTIAL)
		return features;

	/* We cannot support GSO if the MSS is going to be less than
	 * 64 bytes.  If it is then we need to drop support for GSO.
	 */
	if (skb_is_gso(skb) && (skb_shinfo(skb)->gso_size < 64))
		features &= ~NETIF_F_GSO_MASK;

	/* MACLEN can support at most 63 words */
	len = skb_network_header(skb) - skb->data;
	if (len & ~(63 * 2))
		goto out_err;

	/* IPLEN and EIPLEN can support at most 127 dwords */
	len = skb_transport_header(skb) - skb_network_header(skb);
	if (len & ~(127 * 4))
		goto out_err;

	if (skb->encapsulation) {
		/* L4TUNLEN can support 127 words */
		len = skb_inner_network_header(skb) - skb_transport_header(skb);
		if (len & ~(127 * 2))
			goto out_err;

		/* IPLEN can support at most 127 dwords */
		len = skb_inner_transport_header(skb) -
		      skb_inner_network_header(skb);
		if (len & ~(127 * 4))
			goto out_err;
	}

	/* No need to validate L4LEN as TCP is the only protocol with a
	 * a flexible value and we support all possible values supported
	 * by TCP, which is at most 15 dwords
	 */

	return features;
out_err:
	return features & ~(NETIF_F_CSUM_MASK | NETIF_F_GSO_MASK);
}