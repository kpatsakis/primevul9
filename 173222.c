static int bnx2x_get_phys_port_id(struct net_device *netdev,
				  struct netdev_phys_item_id *ppid)
{
	struct bnx2x *bp = netdev_priv(netdev);

	if (!(bp->flags & HAS_PHYS_PORT_ID))
		return -EOPNOTSUPP;

	ppid->id_len = sizeof(bp->phys_port_id);
	memcpy(ppid->id, bp->phys_port_id, ppid->id_len);

	return 0;
}