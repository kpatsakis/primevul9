static int packet_snd(struct socket *sock,
			  struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct sockaddr_ll *saddr = (struct sockaddr_ll *)msg->msg_name;
	struct sk_buff *skb;
	struct net_device *dev;
	__be16 proto;
	bool need_rls_dev = false;
	unsigned char *addr;
	int err, reserve = 0;
	struct virtio_net_hdr vnet_hdr = { 0 };
	int offset = 0;
	int vnet_hdr_len;
	struct packet_sock *po = pkt_sk(sk);
	unsigned short gso_type = 0;

	/*
	 *	Get and verify the address.
	 */

	if (saddr == NULL) {
		dev = po->prot_hook.dev;
		proto	= po->num;
		addr	= NULL;
	} else {
		err = -EINVAL;
		if (msg->msg_namelen < sizeof(struct sockaddr_ll))
			goto out;
		if (msg->msg_namelen < (saddr->sll_halen + offsetof(struct sockaddr_ll, sll_addr)))
			goto out;
		proto	= saddr->sll_protocol;
		addr	= saddr->sll_addr;
		dev = dev_get_by_index(sock_net(sk), saddr->sll_ifindex);
		need_rls_dev = true;
	}

	err = -ENXIO;
	if (dev == NULL)
		goto out_unlock;
	if (sock->type == SOCK_RAW)
		reserve = dev->hard_header_len;

	err = -ENETDOWN;
	if (!(dev->flags & IFF_UP))
		goto out_unlock;

	if (po->has_vnet_hdr) {
		vnet_hdr_len = sizeof(vnet_hdr);

		err = -EINVAL;
		if (len < vnet_hdr_len)
			goto out_unlock;

		len -= vnet_hdr_len;

		err = memcpy_fromiovec((void *)&vnet_hdr, msg->msg_iov,
				       vnet_hdr_len);
		if (err < 0)
			goto out_unlock;

		if ((vnet_hdr.flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) &&
		    (vnet_hdr.csum_start + vnet_hdr.csum_offset + 2 >
		      vnet_hdr.hdr_len))
			vnet_hdr.hdr_len = vnet_hdr.csum_start +
						 vnet_hdr.csum_offset + 2;

		err = -EINVAL;
		if (vnet_hdr.hdr_len > len)
			goto out_unlock;

		if (vnet_hdr.gso_type != VIRTIO_NET_HDR_GSO_NONE) {
			switch (vnet_hdr.gso_type & ~VIRTIO_NET_HDR_GSO_ECN) {
			case VIRTIO_NET_HDR_GSO_TCPV4:
				gso_type = SKB_GSO_TCPV4;
				break;
			case VIRTIO_NET_HDR_GSO_TCPV6:
				gso_type = SKB_GSO_TCPV6;
				break;
			case VIRTIO_NET_HDR_GSO_UDP:
				gso_type = SKB_GSO_UDP;
				break;
			default:
				goto out_unlock;
			}

			if (vnet_hdr.gso_type & VIRTIO_NET_HDR_GSO_ECN)
				gso_type |= SKB_GSO_TCP_ECN;

			if (vnet_hdr.gso_size == 0)
				goto out_unlock;

		}
	}

	err = -EMSGSIZE;
	if (!gso_type && (len > dev->mtu + reserve + VLAN_HLEN))
		goto out_unlock;

	err = -ENOBUFS;
	skb = packet_alloc_skb(sk, LL_ALLOCATED_SPACE(dev),
			       LL_RESERVED_SPACE(dev), len, vnet_hdr.hdr_len,
			       msg->msg_flags & MSG_DONTWAIT, &err);
	if (skb == NULL)
		goto out_unlock;

	skb_set_network_header(skb, reserve);

	err = -EINVAL;
	if (sock->type == SOCK_DGRAM &&
	    (offset = dev_hard_header(skb, dev, ntohs(proto), addr, NULL, len)) < 0)
		goto out_free;

	/* Returns -EFAULT on error */
	err = skb_copy_datagram_from_iovec(skb, offset, msg->msg_iov, 0, len);
	if (err)
		goto out_free;
	err = sock_tx_timestamp(sk, &skb_shinfo(skb)->tx_flags);
	if (err < 0)
		goto out_free;

	if (!gso_type && (len > dev->mtu + reserve)) {
		/* Earlier code assumed this would be a VLAN pkt,
		 * double-check this now that we have the actual
		 * packet in hand.
		 */
		struct ethhdr *ehdr;
		skb_reset_mac_header(skb);
		ehdr = eth_hdr(skb);
		if (ehdr->h_proto != htons(ETH_P_8021Q)) {
			err = -EMSGSIZE;
			goto out_free;
		}
	}

	skb->protocol = proto;
	skb->dev = dev;
	skb->priority = sk->sk_priority;
	skb->mark = sk->sk_mark;

	if (po->has_vnet_hdr) {
		if (vnet_hdr.flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) {
			if (!skb_partial_csum_set(skb, vnet_hdr.csum_start,
						  vnet_hdr.csum_offset)) {
				err = -EINVAL;
				goto out_free;
			}
		}

		skb_shinfo(skb)->gso_size = vnet_hdr.gso_size;
		skb_shinfo(skb)->gso_type = gso_type;

		/* Header must be checked, and gso_segs computed. */
		skb_shinfo(skb)->gso_type |= SKB_GSO_DODGY;
		skb_shinfo(skb)->gso_segs = 0;

		len += vnet_hdr_len;
	}

	/*
	 *	Now send it
	 */

	err = dev_queue_xmit(skb);
	if (err > 0 && (err = net_xmit_errno(err)) != 0)
		goto out_unlock;

	if (need_rls_dev)
		dev_put(dev);

	return len;

out_free:
	kfree_skb(skb);
out_unlock:
	if (dev && need_rls_dev)
		dev_put(dev);
out:
	return err;
}