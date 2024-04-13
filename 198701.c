static void tw5864_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vb2_queue *vq = vb->vb2_queue;
	struct tw5864_input *dev = vb2_get_drv_priv(vq);
	struct tw5864_buf *buf = container_of(vbuf, struct tw5864_buf, vb);
	unsigned long flags;

	spin_lock_irqsave(&dev->slock, flags);
	list_add_tail(&buf->list, &dev->active);
	spin_unlock_irqrestore(&dev->slock, flags);
}