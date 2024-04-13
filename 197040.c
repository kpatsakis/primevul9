static bool bond_time_in_interval(struct bonding *bond, unsigned long last_act,
				  int mod)
{
	int delta_in_ticks = msecs_to_jiffies(bond->params.arp_interval);

	return time_in_range(jiffies,
			     last_act - delta_in_ticks,
			     last_act + mod * delta_in_ticks + delta_in_ticks/2);
}