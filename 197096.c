static netdev_tx_t bond_xmit_broadcast(struct sk_buff *skb,
				       struct net_device *bond_dev)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct slave *slave = NULL;
	struct list_head *iter;

	bond_for_each_slave_rcu(bond, slave, iter) {
		if (bond_is_last_slave(bond, slave))
			break;
		if (bond_slave_is_up(slave) && slave->link == BOND_LINK_UP) {
			struct sk_buff *skb2 = skb_clone(skb, GFP_ATOMIC);

			if (!skb2) {
				net_err_ratelimited("%s: Error: %s: skb_clone() failed\n",
						    bond_dev->name, __func__);
				continue;
			}
			bond_dev_queue_xmit(bond, skb2, slave->dev);
		}
	}
	if (slave && bond_slave_is_up(slave) && slave->link == BOND_LINK_UP)
		return bond_dev_queue_xmit(bond, skb, slave->dev);

	return bond_tx_drop(bond_dev, skb);
}