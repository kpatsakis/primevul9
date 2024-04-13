static struct eth_spe *bnx2x_sp_get_next(struct bnx2x *bp)
{
	struct eth_spe *next_spe = bp->spq_prod_bd;

	if (bp->spq_prod_bd == bp->spq_last_bd) {
		bp->spq_prod_bd = bp->spq;
		bp->spq_prod_idx = 0;
		DP(BNX2X_MSG_SP, "end of spq\n");
	} else {
		bp->spq_prod_bd++;
		bp->spq_prod_idx++;
	}
	return next_spe;
}