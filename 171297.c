vhost_user_get_features(struct virtio_net *dev)
{
	uint64_t features = 0;

	rte_vhost_driver_get_features(dev->ifname, &features);
	return features;
}