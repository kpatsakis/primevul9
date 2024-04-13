static void bnx2x_sp_prod_update(struct bnx2x *bp)
{
	int func = BP_FUNC(bp);

	/*
	 * Make sure that BD data is updated before writing the producer:
	 * BD data is written to the memory, the producer is read from the
	 * memory, thus we need a full memory barrier to ensure the ordering.
	 */
	mb();

	REG_WR16(bp, BAR_XSTRORM_INTMEM + XSTORM_SPQ_PROD_OFFSET(func),
		 bp->spq_prod_idx);
	mmiowb();
}