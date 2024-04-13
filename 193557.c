static void handle_tx(struct vhost_net *net)
{
	struct vhost_virtqueue *vq = &net->dev.vqs[VHOST_NET_VQ_TX];
	unsigned out, in, s;
	int head;
	struct msghdr msg = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_iov = vq->iov,
		.msg_flags = MSG_DONTWAIT,
	};
	size_t len, total_len = 0;
	int err, wmem;
	size_t hdr_size;
	struct socket *sock;
	struct vhost_ubuf_ref *uninitialized_var(ubufs);
	bool zcopy;

	/* TODO: check that we are running from vhost_worker? */
	sock = rcu_dereference_check(vq->private_data, 1);
	if (!sock)
		return;

	wmem = atomic_read(&sock->sk->sk_wmem_alloc);
	if (wmem >= sock->sk->sk_sndbuf) {
		mutex_lock(&vq->mutex);
		tx_poll_start(net, sock);
		mutex_unlock(&vq->mutex);
		return;
	}

	mutex_lock(&vq->mutex);
	vhost_disable_notify(&net->dev, vq);

	if (wmem < sock->sk->sk_sndbuf / 2)
		tx_poll_stop(net);
	hdr_size = vq->vhost_hlen;
	zcopy = vq->ubufs;

	for (;;) {
		/* Release DMAs done buffers first */
		if (zcopy)
			vhost_zerocopy_signal_used(net, vq);

		head = vhost_get_vq_desc(&net->dev, vq, vq->iov,
					 ARRAY_SIZE(vq->iov),
					 &out, &in,
					 NULL, NULL);
		/* On error, stop handling until the next kick. */
		if (unlikely(head < 0))
			break;
		/* Nothing new?  Wait for eventfd to tell us they refilled. */
		if (head == vq->num) {
			int num_pends;

			wmem = atomic_read(&sock->sk->sk_wmem_alloc);
			if (wmem >= sock->sk->sk_sndbuf * 3 / 4) {
				tx_poll_start(net, sock);
				set_bit(SOCK_ASYNC_NOSPACE, &sock->flags);
				break;
			}
			/* If more outstanding DMAs, queue the work.
			 * Handle upend_idx wrap around
			 */
			num_pends = likely(vq->upend_idx >= vq->done_idx) ?
				    (vq->upend_idx - vq->done_idx) :
				    (vq->upend_idx + UIO_MAXIOV - vq->done_idx);
			if (unlikely(num_pends > VHOST_MAX_PEND)) {
				tx_poll_start(net, sock);
				set_bit(SOCK_ASYNC_NOSPACE, &sock->flags);
				break;
			}
			if (unlikely(vhost_enable_notify(&net->dev, vq))) {
				vhost_disable_notify(&net->dev, vq);
				continue;
			}
			break;
		}
		if (in) {
			vq_err(vq, "Unexpected descriptor format for TX: "
			       "out %d, int %d\n", out, in);
			break;
		}
		/* Skip header. TODO: support TSO. */
		s = move_iovec_hdr(vq->iov, vq->hdr, hdr_size, out);
		msg.msg_iovlen = out;
		len = iov_length(vq->iov, out);
		/* Sanity check */
		if (!len) {
			vq_err(vq, "Unexpected header len for TX: "
			       "%zd expected %zd\n",
			       iov_length(vq->hdr, s), hdr_size);
			break;
		}
		/* use msg_control to pass vhost zerocopy ubuf info to skb */
		if (zcopy) {
			vq->heads[vq->upend_idx].id = head;
			if (!vhost_net_tx_select_zcopy(net) ||
			    len < VHOST_GOODCOPY_LEN) {
				/* copy don't need to wait for DMA done */
				vq->heads[vq->upend_idx].len =
							VHOST_DMA_DONE_LEN;
				msg.msg_control = NULL;
				msg.msg_controllen = 0;
				ubufs = NULL;
			} else {
				struct ubuf_info *ubuf = &vq->ubuf_info[head];

				vq->heads[vq->upend_idx].len =
					VHOST_DMA_IN_PROGRESS;
				ubuf->callback = vhost_zerocopy_callback;
				ubuf->ctx = vq->ubufs;
				ubuf->desc = vq->upend_idx;
				msg.msg_control = ubuf;
				msg.msg_controllen = sizeof(ubuf);
				ubufs = vq->ubufs;
				kref_get(&ubufs->kref);
			}
			vq->upend_idx = (vq->upend_idx + 1) % UIO_MAXIOV;
		}
		/* TODO: Check specific error and bomb out unless ENOBUFS? */
		err = sock->ops->sendmsg(NULL, sock, &msg, len);
		if (unlikely(err < 0)) {
			if (zcopy) {
				if (ubufs)
					vhost_ubuf_put(ubufs);
				vq->upend_idx = ((unsigned)vq->upend_idx - 1) %
					UIO_MAXIOV;
			}
			vhost_discard_vq_desc(vq, 1);
			if (err == -EAGAIN || err == -ENOBUFS)
				tx_poll_start(net, sock);
			break;
		}
		if (err != len)
			pr_debug("Truncated TX packet: "
				 " len %d != %zd\n", err, len);
		if (!zcopy)
			vhost_add_used_and_signal(&net->dev, vq, head, 0);
		else
			vhost_zerocopy_signal_used(net, vq);
		total_len += len;
		vhost_net_tx_packet(net);
		if (unlikely(total_len >= VHOST_NET_WEIGHT)) {
			vhost_poll_queue(&vq->poll);
			break;
		}
	}

	mutex_unlock(&vq->mutex);
}