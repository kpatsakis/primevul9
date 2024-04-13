__nfqnl_enqueue_packet(struct net *net, struct nfqnl_instance *queue,
			struct nf_queue_entry *entry)
{
	struct sk_buff *nskb;
	int err = -ENOBUFS;
	__be32 *packet_id_ptr;
	int failopen = 0;

	nskb = nfqnl_build_packet_message(net, queue, entry, &packet_id_ptr);
	if (nskb == NULL) {
		err = -ENOMEM;
		goto err_out;
	}
	spin_lock_bh(&queue->lock);

	if (queue->queue_total >= queue->queue_maxlen) {
		if (queue->flags & NFQA_CFG_F_FAIL_OPEN) {
			failopen = 1;
			err = 0;
		} else {
			queue->queue_dropped++;
			net_warn_ratelimited("nf_queue: full at %d entries, dropping packets(s)\n",
					     queue->queue_total);
		}
		goto err_out_free_nskb;
	}
	entry->id = ++queue->id_sequence;
	*packet_id_ptr = htonl(entry->id);

	/* nfnetlink_unicast will either free the nskb or add it to a socket */
	err = nfnetlink_unicast(nskb, net, queue->peer_portid, MSG_DONTWAIT);
	if (err < 0) {
		queue->queue_user_dropped++;
		goto err_out_unlock;
	}

	__enqueue_entry(queue, entry);

	spin_unlock_bh(&queue->lock);
	return 0;

err_out_free_nskb:
	kfree_skb(nskb);
err_out_unlock:
	spin_unlock_bh(&queue->lock);
	if (failopen)
		nf_reinject(entry, NF_ACCEPT);
err_out:
	return err;
}