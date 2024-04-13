static int queue_gso_packets(struct datapath *dp, struct sk_buff *skb,
			     const struct dp_upcall_info *upcall_info)
{
	unsigned short gso_type = skb_shinfo(skb)->gso_type;
	struct dp_upcall_info later_info;
	struct sw_flow_key later_key;
	struct sk_buff *segs, *nskb;
	int err;

	segs = __skb_gso_segment(skb, NETIF_F_SG, false);
	if (IS_ERR(segs))
		return PTR_ERR(segs);

	/* Queue all of the segments. */
	skb = segs;
	do {
		err = queue_userspace_packet(dp, skb, upcall_info);
		if (err)
			break;

		if (skb == segs && gso_type & SKB_GSO_UDP) {
			/* The initial flow key extracted by ovs_flow_extract()
			 * in this case is for a first fragment, so we need to
			 * properly mark later fragments.
			 */
			later_key = *upcall_info->key;
			later_key.ip.frag = OVS_FRAG_TYPE_LATER;

			later_info = *upcall_info;
			later_info.key = &later_key;
			upcall_info = &later_info;
		}
	} while ((skb = skb->next));

	/* Free all of the segments. */
	skb = segs;
	do {
		nskb = skb->next;
		if (err)
			kfree_skb(skb);
		else
			consume_skb(skb);
	} while ((skb = nskb));
	return err;
}