is_vring_iotlb_invalidate(struct vhost_virtqueue *vq,
				struct vhost_iotlb_msg *imsg)
{
	uint64_t istart, iend, vstart, vend;

	istart = imsg->iova;
	iend = istart + imsg->size - 1;

	vstart = (uintptr_t)vq->desc;
	vend = vstart + sizeof(struct vring_desc) * vq->size - 1;
	if (vstart <= iend && istart <= vend)
		return 1;

	vstart = (uintptr_t)vq->avail;
	vend = vstart + sizeof(struct vring_avail);
	vend += sizeof(uint16_t) * vq->size - 1;
	if (vstart <= iend && istart <= vend)
		return 1;

	vstart = (uintptr_t)vq->used;
	vend = vstart + sizeof(struct vring_used);
	vend += sizeof(struct vring_used_elem) * vq->size - 1;
	if (vstart <= iend && istart <= vend)
		return 1;

	return 0;
}