static void bond_setup_by_slave(struct net_device *bond_dev,
				struct net_device *slave_dev)
{
	bond_dev->header_ops	    = slave_dev->header_ops;

	bond_dev->type		    = slave_dev->type;
	bond_dev->hard_header_len   = slave_dev->hard_header_len;
	bond_dev->needed_headroom   = slave_dev->needed_headroom;
	bond_dev->addr_len	    = slave_dev->addr_len;

	memcpy(bond_dev->broadcast, slave_dev->broadcast,
		slave_dev->addr_len);
}