static int qrtr_tun_open(struct inode *inode, struct file *filp)
{
	struct qrtr_tun *tun;

	tun = kzalloc(sizeof(*tun), GFP_KERNEL);
	if (!tun)
		return -ENOMEM;

	skb_queue_head_init(&tun->queue);
	init_waitqueue_head(&tun->readq);

	tun->ep.xmit = qrtr_tun_send;

	filp->private_data = tun;

	return qrtr_endpoint_register(&tun->ep, QRTR_EP_NID_AUTO);
}