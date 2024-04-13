static int tpacket_snd(struct packet_sock *po, struct msghdr *msg)
{
	struct sk_buff *skb;
	struct net_device *dev;
	__be16 proto;
	bool need_rls_dev = false;
	int err, reserve = 0;
	void *ph;
	struct sockaddr_ll *saddr = (struct sockaddr_ll *)msg->msg_name;
	int tp_len, size_max;
	unsigned char *addr;
	int len_sum = 0;
	int status = 0;

	mutex_lock(&po->pg_vec_lock);

	err = -EBUSY;
	if (saddr == NULL) {
		dev = po->prot_hook.dev;
		proto	= po->num;
		addr	= NULL;
	} else {
		err = -EINVAL;
		if (msg->msg_namelen < sizeof(struct sockaddr_ll))
			goto out;
		if (msg->msg_namelen < (saddr->sll_halen
					+ offsetof(struct sockaddr_ll,
						sll_addr)))
			goto out;
		proto	= saddr->sll_protocol;
		addr	= saddr->sll_addr;
		dev = dev_get_by_index(sock_net(&po->sk), saddr->sll_ifindex);
		need_rls_dev = true;
	}

	err = -ENXIO;
	if (unlikely(dev == NULL))
		goto out;

	reserve = dev->hard_header_len;

	err = -ENETDOWN;
	if (unlikely(!(dev->flags & IFF_UP)))
		goto out_put;

	size_max = po->tx_ring.frame_size
		- (po->tp_hdrlen - sizeof(struct sockaddr_ll));

	if (size_max > dev->mtu + reserve)
		size_max = dev->mtu + reserve;

	do {
		ph = packet_current_frame(po, &po->tx_ring,
				TP_STATUS_SEND_REQUEST);

		if (unlikely(ph == NULL)) {
			schedule();
			continue;
		}

		status = TP_STATUS_SEND_REQUEST;
		skb = sock_alloc_send_skb(&po->sk,
				LL_ALLOCATED_SPACE(dev)
				+ sizeof(struct sockaddr_ll),
				0, &err);

		if (unlikely(skb == NULL))
			goto out_status;

		tp_len = tpacket_fill_skb(po, skb, ph, dev, size_max, proto,
				addr);

		if (unlikely(tp_len < 0)) {
			if (po->tp_loss) {
				__packet_set_status(po, ph,
						TP_STATUS_AVAILABLE);
				packet_increment_head(&po->tx_ring);
				kfree_skb(skb);
				continue;
			} else {
				status = TP_STATUS_WRONG_FORMAT;
				err = tp_len;
				goto out_status;
			}
		}

		skb->destructor = tpacket_destruct_skb;
		__packet_set_status(po, ph, TP_STATUS_SENDING);
		atomic_inc(&po->tx_ring.pending);

		status = TP_STATUS_SEND_REQUEST;
		err = dev_queue_xmit(skb);
		if (unlikely(err > 0)) {
			err = net_xmit_errno(err);
			if (err && __packet_get_status(po, ph) ==
				   TP_STATUS_AVAILABLE) {
				/* skb was destructed already */
				skb = NULL;
				goto out_status;
			}
			/*
			 * skb was dropped but not destructed yet;
			 * let's treat it like congestion or err < 0
			 */
			err = 0;
		}
		packet_increment_head(&po->tx_ring);
		len_sum += tp_len;
	} while (likely((ph != NULL) ||
			((!(msg->msg_flags & MSG_DONTWAIT)) &&
			 (atomic_read(&po->tx_ring.pending))))
		);

	err = len_sum;
	goto out_put;

out_status:
	__packet_set_status(po, ph, status);
	kfree_skb(skb);
out_put:
	if (need_rls_dev)
		dev_put(dev);
out:
	mutex_unlock(&po->pg_vec_lock);
	return err;
}