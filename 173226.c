static int bnx2x_set_iscsi_eth_mac_addr(struct bnx2x *bp)
{
	unsigned long ramrod_flags = 0;

	__set_bit(RAMROD_COMP_WAIT, &ramrod_flags);
	return bnx2x_set_mac_one(bp, bp->cnic_eth_dev.iscsi_mac,
				 &bp->iscsi_l2_mac_obj, true,
				 BNX2X_ISCSI_ETH_MAC, &ramrod_flags);
}