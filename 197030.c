void bond_work_init_all(struct bonding *bond)
{
	INIT_DELAYED_WORK(&bond->mcast_work,
			  bond_resend_igmp_join_requests_delayed);
	INIT_DELAYED_WORK(&bond->alb_work, bond_alb_monitor);
	INIT_DELAYED_WORK(&bond->mii_work, bond_mii_monitor);
	INIT_DELAYED_WORK(&bond->arp_work, bond_arp_monitor);
	INIT_DELAYED_WORK(&bond->ad_work, bond_3ad_state_machine_handler);
	INIT_DELAYED_WORK(&bond->slave_arr_work, bond_slave_arr_handler);
}