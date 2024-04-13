void in6_dev_finish_destroy(struct inet6_dev *idev)
{
	struct net_device *dev = idev->dev;

	WARN_ON(!list_empty(&idev->addr_list));
	WARN_ON(idev->mc_list != NULL);
	WARN_ON(timer_pending(&idev->rs_timer));

#ifdef NET_REFCNT_DEBUG
	pr_debug("%s: %s\n", __func__, dev ? dev->name : "NIL");
#endif
	dev_put(dev);
	if (!idev->dead) {
		pr_warn("Freeing alive inet6 device %p\n", idev);
		return;
	}
	snmp6_free_dev(idev);
	kfree_rcu(idev, rcu);
}