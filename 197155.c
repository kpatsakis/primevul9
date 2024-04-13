static void bond_arp_monitor(struct work_struct *work)
{
	struct bonding *bond = container_of(work, struct bonding,
					    arp_work.work);

	if (BOND_MODE(bond) == BOND_MODE_ACTIVEBACKUP)
		bond_activebackup_arp_mon(bond);
	else
		bond_loadbalance_arp_mon(bond);
}