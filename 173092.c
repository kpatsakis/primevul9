static int bnx2x_register_cnic(struct net_device *dev, struct cnic_ops *ops,
			       void *data)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;
	int rc;

	DP(NETIF_MSG_IFUP, "Register_cnic called\n");

	if (ops == NULL) {
		BNX2X_ERR("NULL ops received\n");
		return -EINVAL;
	}

	if (!CNIC_SUPPORT(bp)) {
		BNX2X_ERR("Can't register CNIC when not supported\n");
		return -EOPNOTSUPP;
	}

	if (!CNIC_LOADED(bp)) {
		rc = bnx2x_load_cnic(bp);
		if (rc) {
			BNX2X_ERR("CNIC-related load failed\n");
			return rc;
		}
	}

	bp->cnic_enabled = true;

	bp->cnic_kwq = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!bp->cnic_kwq)
		return -ENOMEM;

	bp->cnic_kwq_cons = bp->cnic_kwq;
	bp->cnic_kwq_prod = bp->cnic_kwq;
	bp->cnic_kwq_last = bp->cnic_kwq + MAX_SP_DESC_CNT;

	bp->cnic_spq_pending = 0;
	bp->cnic_kwq_pending = 0;

	bp->cnic_data = data;

	cp->num_irq = 0;
	cp->drv_state |= CNIC_DRV_STATE_REGD;
	cp->iro_arr = bp->iro_arr;

	bnx2x_setup_cnic_irq_info(bp);

	rcu_assign_pointer(bp->cnic_ops, ops);

	/* Schedule driver to read CNIC driver versions */
	bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_GET_DRV_VERSION, 0);

	return 0;
}