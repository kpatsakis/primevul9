vhost_user_msg_handler(int vid, int fd)
{
	struct virtio_net *dev;
	struct VhostUserMsg msg;
	int ret;
	int unlock_required = 0;

	dev = get_device(vid);
	if (dev == NULL)
		return -1;

	if (!dev->notify_ops) {
		dev->notify_ops = vhost_driver_callback_get(dev->ifname);
		if (!dev->notify_ops) {
			RTE_LOG(ERR, VHOST_CONFIG,
				"failed to get callback ops for driver %s\n",
				dev->ifname);
			return -1;
		}
	}

	ret = read_vhost_message(fd, &msg);
	if (ret <= 0 || msg.request.master >= VHOST_USER_MAX) {
		if (ret < 0)
			RTE_LOG(ERR, VHOST_CONFIG,
				"vhost read message failed\n");
		else if (ret == 0)
			RTE_LOG(INFO, VHOST_CONFIG,
				"vhost peer closed\n");
		else
			RTE_LOG(ERR, VHOST_CONFIG,
				"vhost read incorrect message\n");

		return -1;
	}

	ret = 0;
	if (msg.request.master != VHOST_USER_IOTLB_MSG)
		RTE_LOG(INFO, VHOST_CONFIG, "read message %s\n",
			vhost_message_str[msg.request.master]);
	else
		RTE_LOG(DEBUG, VHOST_CONFIG, "read message %s\n",
			vhost_message_str[msg.request.master]);

	ret = vhost_user_check_and_alloc_queue_pair(dev, &msg);
	if (ret < 0) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"failed to alloc queue\n");
		return -1;
	}

	/*
	 * Note: we don't lock all queues on VHOST_USER_GET_VRING_BASE
	 * and VHOST_USER_RESET_OWNER, since it is sent when virtio stops
	 * and device is destroyed. destroy_device waits for queues to be
	 * inactive, so it is safe. Otherwise taking the access_lock
	 * would cause a dead lock.
	 */
	switch (msg.request.master) {
	case VHOST_USER_SET_FEATURES:
	case VHOST_USER_SET_PROTOCOL_FEATURES:
	case VHOST_USER_SET_OWNER:
	case VHOST_USER_SET_MEM_TABLE:
	case VHOST_USER_SET_LOG_BASE:
	case VHOST_USER_SET_LOG_FD:
	case VHOST_USER_SET_VRING_NUM:
	case VHOST_USER_SET_VRING_ADDR:
	case VHOST_USER_SET_VRING_BASE:
	case VHOST_USER_SET_VRING_KICK:
	case VHOST_USER_SET_VRING_CALL:
	case VHOST_USER_SET_VRING_ERR:
	case VHOST_USER_SET_VRING_ENABLE:
	case VHOST_USER_SEND_RARP:
	case VHOST_USER_NET_SET_MTU:
	case VHOST_USER_SET_SLAVE_REQ_FD:
		vhost_user_lock_all_queue_pairs(dev);
		unlock_required = 1;
		break;
	default:
		break;

	}

	switch (msg.request.master) {
	case VHOST_USER_GET_FEATURES:
		msg.payload.u64 = vhost_user_get_features(dev);
		msg.size = sizeof(msg.payload.u64);
		send_vhost_reply(fd, &msg);
		break;
	case VHOST_USER_SET_FEATURES:
		ret = vhost_user_set_features(dev, msg.payload.u64);
		if (ret)
			return -1;
		break;

	case VHOST_USER_GET_PROTOCOL_FEATURES:
		vhost_user_get_protocol_features(dev, &msg);
		send_vhost_reply(fd, &msg);
		break;
	case VHOST_USER_SET_PROTOCOL_FEATURES:
		vhost_user_set_protocol_features(dev, msg.payload.u64);
		break;

	case VHOST_USER_SET_OWNER:
		vhost_user_set_owner();
		break;
	case VHOST_USER_RESET_OWNER:
		vhost_user_reset_owner(dev);
		break;

	case VHOST_USER_SET_MEM_TABLE:
		ret = vhost_user_set_mem_table(dev, &msg);
		break;

	case VHOST_USER_SET_LOG_BASE:
		vhost_user_set_log_base(dev, &msg);

		/* it needs a reply */
		msg.size = sizeof(msg.payload.u64);
		send_vhost_reply(fd, &msg);
		break;
	case VHOST_USER_SET_LOG_FD:
		close(msg.fds[0]);
		RTE_LOG(INFO, VHOST_CONFIG, "not implemented.\n");
		break;

	case VHOST_USER_SET_VRING_NUM:
		vhost_user_set_vring_num(dev, &msg);
		break;
	case VHOST_USER_SET_VRING_ADDR:
		vhost_user_set_vring_addr(&dev, &msg);
		break;
	case VHOST_USER_SET_VRING_BASE:
		vhost_user_set_vring_base(dev, &msg);
		break;

	case VHOST_USER_GET_VRING_BASE:
		vhost_user_get_vring_base(dev, &msg);
		msg.size = sizeof(msg.payload.state);
		send_vhost_reply(fd, &msg);
		break;

	case VHOST_USER_SET_VRING_KICK:
		vhost_user_set_vring_kick(&dev, &msg);
		break;
	case VHOST_USER_SET_VRING_CALL:
		vhost_user_set_vring_call(dev, &msg);
		break;

	case VHOST_USER_SET_VRING_ERR:
		if (!(msg.payload.u64 & VHOST_USER_VRING_NOFD_MASK))
			close(msg.fds[0]);
		RTE_LOG(INFO, VHOST_CONFIG, "not implemented\n");
		break;

	case VHOST_USER_GET_QUEUE_NUM:
		msg.payload.u64 = VHOST_MAX_QUEUE_PAIRS;
		msg.size = sizeof(msg.payload.u64);
		send_vhost_reply(fd, &msg);
		break;

	case VHOST_USER_SET_VRING_ENABLE:
		vhost_user_set_vring_enable(dev, &msg);
		break;
	case VHOST_USER_SEND_RARP:
		vhost_user_send_rarp(dev, &msg);
		break;

	case VHOST_USER_NET_SET_MTU:
		ret = vhost_user_net_set_mtu(dev, &msg);
		break;

	case VHOST_USER_SET_SLAVE_REQ_FD:
		ret = vhost_user_set_req_fd(dev, &msg);
		break;

	case VHOST_USER_IOTLB_MSG:
		ret = vhost_user_iotlb_msg(&dev, &msg);
		break;

	default:
		ret = -1;
		break;

	}

	if (unlock_required)
		vhost_user_unlock_all_queue_pairs(dev);

	if (msg.flags & VHOST_USER_NEED_REPLY) {
		msg.payload.u64 = !!ret;
		msg.size = sizeof(msg.payload.u64);
		send_vhost_reply(fd, &msg);
	}

	if (!(dev->flags & VIRTIO_DEV_RUNNING) && virtio_is_ready(dev)) {
		dev->flags |= VIRTIO_DEV_READY;

		if (!(dev->flags & VIRTIO_DEV_RUNNING)) {
			if (dev->dequeue_zero_copy) {
				RTE_LOG(INFO, VHOST_CONFIG,
						"dequeue zero copy is enabled\n");
			}

			if (dev->notify_ops->new_device(dev->vid) == 0)
				dev->flags |= VIRTIO_DEV_RUNNING;
		}
	}

	return 0;
}