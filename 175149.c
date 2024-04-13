__nfqnl_enqueue_packet_gso(struct net *net, struct nfqnl_instance *queue,
			   struct sk_buff *skb, struct nf_queue_entry *entry)
{
	int ret = -ENOMEM;
	struct nf_queue_entry *entry_seg;

	nf_bridge_adjust_segmented_data(skb);

	if (skb->next == NULL) { /* last packet, no need to copy entry */
		struct sk_buff *gso_skb = entry->skb;
		entry->skb = skb;
		ret = __nfqnl_enqueue_packet(net, queue, entry);
		if (ret)
			entry->skb = gso_skb;
		return ret;
	}

	skb->next = NULL;

	entry_seg = nf_queue_entry_dup(entry);
	if (entry_seg) {
		entry_seg->skb = skb;
		ret = __nfqnl_enqueue_packet(net, queue, entry_seg);
		if (ret)
			free_entry(entry_seg);
	}
	return ret;
}