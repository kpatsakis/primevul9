is_vring_iotlb_update(struct vhost_virtqueue *vq, struct vhost_iotlb_msg *imsg)
{
	struct vhost_vring_addr *ra;
	uint64_t start, end;

	start = imsg->iova;
	end = start + imsg->size;

	ra = &vq->ring_addrs;
	if (ra->desc_user_addr >= start && ra->desc_user_addr < end)
		return 1;
	if (ra->avail_user_addr >= start && ra->avail_user_addr < end)
		return 1;
	if (ra->used_user_addr >= start && ra->used_user_addr < end)
		return 1;

	return 0;
}