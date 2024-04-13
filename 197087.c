static void bond_arp_send_all(struct bonding *bond, struct slave *slave)
{
	struct rtable *rt;
	struct bond_vlan_tag *tags;
	__be32 *targets = bond->params.arp_targets, addr;
	int i;

	for (i = 0; i < BOND_MAX_ARP_TARGETS && targets[i]; i++) {
		slave_dbg(bond->dev, slave->dev, "%s: target %pI4\n",
			  __func__, &targets[i]);
		tags = NULL;

		/* Find out through which dev should the packet go */
		rt = ip_route_output(dev_net(bond->dev), targets[i], 0,
				     RTO_ONLINK, 0);
		if (IS_ERR(rt)) {
			/* there's no route to target - try to send arp
			 * probe to generate any traffic (arp_validate=0)
			 */
			if (bond->params.arp_validate)
				net_warn_ratelimited("%s: no route to arp_ip_target %pI4 and arp_validate is set\n",
						     bond->dev->name,
						     &targets[i]);
			bond_arp_send(slave, ARPOP_REQUEST, targets[i],
				      0, tags);
			continue;
		}

		/* bond device itself */
		if (rt->dst.dev == bond->dev)
			goto found;

		rcu_read_lock();
		tags = bond_verify_device_path(bond->dev, rt->dst.dev, 0);
		rcu_read_unlock();

		if (!IS_ERR_OR_NULL(tags))
			goto found;

		/* Not our device - skip */
		slave_dbg(bond->dev, slave->dev, "no path to arp_ip_target %pI4 via rt.dev %s\n",
			   &targets[i], rt->dst.dev ? rt->dst.dev->name : "NULL");

		ip_rt_put(rt);
		continue;

found:
		addr = bond_confirm_addr(rt->dst.dev, targets[i], 0);
		ip_rt_put(rt);
		bond_arp_send(slave, ARPOP_REQUEST, targets[i], addr, tags);
		kfree(tags);
	}
}