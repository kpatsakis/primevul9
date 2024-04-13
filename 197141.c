static void bond_netdev_notify_work(struct work_struct *_work)
{
	struct slave *slave = container_of(_work, struct slave,
					   notify_work.work);

	if (rtnl_trylock()) {
		struct netdev_bonding_info binfo;

		bond_fill_ifslave(slave, &binfo.slave);
		bond_fill_ifbond(slave->bond, &binfo.master);
		netdev_bonding_info_change(slave->dev, &binfo);
		rtnl_unlock();
	} else {
		queue_delayed_work(slave->bond->wq, &slave->notify_work, 1);
	}
}