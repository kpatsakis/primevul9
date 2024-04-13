static void bnx2x_init_tx_rings_cnic(struct bnx2x *bp)
{
	int i;

	for_each_tx_queue_cnic(bp, i)
		bnx2x_init_tx_ring_one(bp->fp[i].txdata_ptr[0]);
}