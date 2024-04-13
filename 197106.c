static int bond_set_dev_addr(struct net_device *bond_dev,
			     struct net_device *slave_dev)
{
	int err;

	slave_dbg(bond_dev, slave_dev, "bond_dev=%p slave_dev=%p slave_dev->addr_len=%d\n",
		  bond_dev, slave_dev, slave_dev->addr_len);
	err = dev_pre_changeaddr_notify(bond_dev, slave_dev->dev_addr, NULL);
	if (err)
		return err;

	memcpy(bond_dev->dev_addr, slave_dev->dev_addr, slave_dev->addr_len);
	bond_dev->addr_assign_type = NET_ADDR_STOLEN;
	call_netdevice_notifiers(NETDEV_CHANGEADDR, bond_dev);
	return 0;
}