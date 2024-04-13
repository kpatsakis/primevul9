static void bond_resend_igmp_join_requests_delayed(struct work_struct *work)
{
	struct bonding *bond = container_of(work, struct bonding,
					    mcast_work.work);

	if (!rtnl_trylock()) {
		queue_delayed_work(bond->wq, &bond->mcast_work, 1);
		return;
	}
	call_netdevice_notifiers(NETDEV_RESEND_IGMP, bond->dev);

	if (bond->igmp_retrans > 1) {
		bond->igmp_retrans--;
		queue_delayed_work(bond->wq, &bond->mcast_work, HZ/5);
	}
	rtnl_unlock();
}