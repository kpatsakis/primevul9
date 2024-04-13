
static int ql_cycle_adapter(struct ql3_adapter *qdev, int reset)
{
	if (ql_adapter_down(qdev, reset) || ql_adapter_up(qdev)) {
		netdev_err(qdev->ndev,
			   "Driver up/down cycle failed, closing device\n");
		rtnl_lock();
		dev_close(qdev->ndev);
		rtnl_unlock();
		return -1;
	}
	return 0;