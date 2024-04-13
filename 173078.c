int bnx2x_set_eth_mac(struct bnx2x *bp, bool set)
{
	if (IS_PF(bp)) {
		unsigned long ramrod_flags = 0;

		DP(NETIF_MSG_IFUP, "Adding Eth MAC\n");
		__set_bit(RAMROD_COMP_WAIT, &ramrod_flags);
		return bnx2x_set_mac_one(bp, bp->dev->dev_addr,
					 &bp->sp_objs->mac_obj, set,
					 BNX2X_ETH_MAC, &ramrod_flags);
	} else { /* vf */
		return bnx2x_vfpf_config_mac(bp, bp->dev->dev_addr,
					     bp->fp->index, set);
	}
}