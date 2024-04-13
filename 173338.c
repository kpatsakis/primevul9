static void bnx2x_init_sp_ring(struct bnx2x *bp)
{
	spin_lock_init(&bp->spq_lock);
	atomic_set(&bp->cq_spq_left, MAX_SPQ_PENDING);

	bp->spq_prod_idx = 0;
	bp->dsb_sp_prod = BNX2X_SP_DSB_INDEX;
	bp->spq_prod_bd = bp->spq;
	bp->spq_last_bd = bp->spq_prod_bd + MAX_SP_DESC_CNT;
}