static int qrtr_tun_release(struct inode *inode, struct file *filp)
{
	struct qrtr_tun *tun = filp->private_data;
	struct sk_buff *skb;

	qrtr_endpoint_unregister(&tun->ep);

	/* Discard all SKBs */
	while (!skb_queue_empty(&tun->queue)) {
		skb = skb_dequeue(&tun->queue);
		kfree_skb(skb);
	}

	kfree(tun);

	return 0;
}