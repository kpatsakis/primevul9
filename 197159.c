static inline int bond_slave_override(struct bonding *bond,
				      struct sk_buff *skb)
{
	struct slave *slave = NULL;
	struct list_head *iter;

	if (!skb_rx_queue_recorded(skb))
		return 1;

	/* Find out if any slaves have the same mapping as this skb. */
	bond_for_each_slave_rcu(bond, slave, iter) {
		if (slave->queue_id == skb_get_queue_mapping(skb)) {
			if (bond_slave_is_up(slave) &&
			    slave->link == BOND_LINK_UP) {
				bond_dev_queue_xmit(bond, skb, slave->dev);
				return 0;
			}
			/* If the slave isn't UP, use default transmit policy. */
			break;
		}
	}

	return 1;
}