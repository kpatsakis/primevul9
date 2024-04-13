
static void ql_set_msglevel(struct net_device *ndev, u32 value)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	qdev->msg_enable = value;