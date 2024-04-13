static int rpmsg_eptdev_release(struct inode *inode, struct file *filp)
{
	struct rpmsg_eptdev *eptdev = cdev_to_eptdev(inode->i_cdev);
	struct device *dev = &eptdev->dev;
	struct sk_buff *skb;

	/* Close the endpoint, if it's not already destroyed by the parent */
	mutex_lock(&eptdev->ept_lock);
	if (eptdev->ept) {
		rpmsg_destroy_ept(eptdev->ept);
		eptdev->ept = NULL;
	}
	mutex_unlock(&eptdev->ept_lock);

	/* Discard all SKBs */
	while (!skb_queue_empty(&eptdev->queue)) {
		skb = skb_dequeue(&eptdev->queue);
		kfree_skb(skb);
	}

	put_device(dev);

	return 0;
}