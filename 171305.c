ring_addr_to_vva(struct virtio_net *dev, struct vhost_virtqueue *vq,
		uint64_t ra, uint64_t size)
{
	if (dev->features & (1ULL << VIRTIO_F_IOMMU_PLATFORM)) {
		uint64_t vva;

		vva = vhost_user_iotlb_cache_find(vq, ra,
					&size, VHOST_ACCESS_RW);
		if (!vva)
			vhost_user_iotlb_miss(dev, ra, VHOST_ACCESS_RW);

		return vva;
	}

	return qva_to_vva(dev, ra);
}