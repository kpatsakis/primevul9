static int bnx2x_unregister_cnic(struct net_device *dev)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	mutex_lock(&bp->cnic_mutex);
	cp->drv_state = 0;
	RCU_INIT_POINTER(bp->cnic_ops, NULL);
	mutex_unlock(&bp->cnic_mutex);
	synchronize_rcu();
	bp->cnic_enabled = false;
	kfree(bp->cnic_kwq);
	bp->cnic_kwq = NULL;

	return 0;
}