
static u32 ql_get_msglevel(struct net_device *ndev)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	return qdev->msg_enable;