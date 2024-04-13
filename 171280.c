vhost_user_reset_owner(struct virtio_net *dev)
{
	if (dev->flags & VIRTIO_DEV_RUNNING) {
		dev->flags &= ~VIRTIO_DEV_RUNNING;
		dev->notify_ops->destroy_device(dev->vid);
	}

	cleanup_device(dev, 0);
	reset_device(dev);
	return 0;
}