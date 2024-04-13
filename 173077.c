static void bnx2x_init_tx_rings(struct bnx2x *bp)
{
	int i;
	u8 cos;

	for_each_eth_queue(bp, i)
		for_each_cos_in_tx_queue(&bp->fp[i], cos)
			bnx2x_init_tx_ring_one(bp->fp[i].txdata_ptr[cos]);
}