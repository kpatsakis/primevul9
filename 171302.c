vq_is_ready(struct vhost_virtqueue *vq)
{
	return vq && vq->desc && vq->avail && vq->used &&
	       vq->kickfd != VIRTIO_UNINITIALIZED_EVENTFD &&
	       vq->callfd != VIRTIO_UNINITIALIZED_EVENTFD;
}