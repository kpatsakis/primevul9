static inline void sdma_update_tail(struct sdma_engine *sde, u16 tail)
{
	/* Commit writes to memory and advance the tail on the chip */
	smp_wmb(); /* see get_txhead() */
	writeq(tail, sde->tail_csr);
}