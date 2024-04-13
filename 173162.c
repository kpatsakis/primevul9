void bnx2x_update_coalesce(struct bnx2x *bp)
{
	int i;

	for_each_eth_queue(bp, i)
		bnx2x_update_coalesce_sb(bp, bp->fp[i].fw_sb_id,
					 bp->tx_ticks, bp->rx_ticks);
}