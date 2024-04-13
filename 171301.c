vhost_user_set_req_fd(struct virtio_net *dev, struct VhostUserMsg *msg)
{
	int fd = msg->fds[0];

	if (fd < 0) {
		RTE_LOG(ERR, VHOST_CONFIG,
				"Invalid file descriptor for slave channel (%d)\n",
				fd);
		return -1;
	}

	dev->slave_req_fd = fd;

	return 0;
}