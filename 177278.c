static void sdma_setlengen(struct sdma_engine *sde)
{
#ifdef CONFIG_SDMA_VERBOSITY
	dd_dev_err(sde->dd, "CONFIG SDMA(%u) %s:%d %s()\n",
		   sde->this_idx, slashstrip(__FILE__), __LINE__, __func__);
#endif

	/*
	 * Set SendDmaLenGen and clear-then-set the MSB of the generation
	 * count to enable generation checking and load the internal
	 * generation counter.
	 */
	write_sde_csr(sde, SD(LEN_GEN),
		      (sde->descq_cnt / 64) << SD(LEN_GEN_LENGTH_SHIFT));
	write_sde_csr(sde, SD(LEN_GEN),
		      ((sde->descq_cnt / 64) << SD(LEN_GEN_LENGTH_SHIFT)) |
		      (4ULL << SD(LEN_GEN_GENERATION_SHIFT)));
}