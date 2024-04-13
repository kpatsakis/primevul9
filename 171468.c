
static int ql3xxx_open(struct net_device *ndev)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	return ql_adapter_up(qdev);