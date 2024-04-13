
static int ql3xxx_close(struct net_device *ndev)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);

	/*
	 * Wait for device to recover from a reset.
	 * (Rarely happens, but possible.)
	 */
	while (!test_bit(QL_ADAPTER_UP, &qdev->flags))
		msleep(50);

	ql_adapter_down(qdev, QL_DO_RESET);
	return 0;