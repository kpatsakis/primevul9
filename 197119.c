static void bond_validate_arp(struct bonding *bond, struct slave *slave, __be32 sip, __be32 tip)
{
	int i;

	if (!sip || !bond_has_this_ip(bond, tip)) {
		slave_dbg(bond->dev, slave->dev, "%s: sip %pI4 tip %pI4 not found\n",
			   __func__, &sip, &tip);
		return;
	}

	i = bond_get_targets_ip(bond->params.arp_targets, sip);
	if (i == -1) {
		slave_dbg(bond->dev, slave->dev, "%s: sip %pI4 not found in targets\n",
			   __func__, &sip);
		return;
	}
	slave->last_rx = jiffies;
	slave->target_last_arp_rx[i] = jiffies;
}