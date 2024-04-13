static u32 bond_rr_gen_slave_id(struct bonding *bond)
{
	u32 slave_id;
	struct reciprocal_value reciprocal_packets_per_slave;
	int packets_per_slave = bond->params.packets_per_slave;

	switch (packets_per_slave) {
	case 0:
		slave_id = prandom_u32();
		break;
	case 1:
		slave_id = this_cpu_inc_return(*bond->rr_tx_counter);
		break;
	default:
		reciprocal_packets_per_slave =
			bond->params.reciprocal_packets_per_slave;
		slave_id = this_cpu_inc_return(*bond->rr_tx_counter);
		slave_id = reciprocal_divide(slave_id,
					     reciprocal_packets_per_slave);
		break;
	}

	return slave_id;
}