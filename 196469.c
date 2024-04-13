static int rpmsg_ept_cb(struct rpmsg_device *rpdev, void *buf, int len,
			void *priv, u32 addr)
{
	struct rpmsg_eptdev *eptdev = priv;
	struct sk_buff *skb;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	skb_put_data(skb, buf, len);

	spin_lock(&eptdev->queue_lock);
	skb_queue_tail(&eptdev->queue, skb);
	spin_unlock(&eptdev->queue_lock);

	/* wake up any blocking processes, waiting for new data */
	wake_up_interruptible(&eptdev->readq);

	return 0;
}