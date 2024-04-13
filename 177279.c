static void sdma_hw_start_up(struct sdma_engine *sde)
{
	u64 reg;

#ifdef CONFIG_SDMA_VERBOSITY
	dd_dev_err(sde->dd, "CONFIG SDMA(%u) %s:%d %s()\n",
		   sde->this_idx, slashstrip(__FILE__), __LINE__, __func__);
#endif

	sdma_setlengen(sde);
	sdma_update_tail(sde, 0); /* Set SendDmaTail */
	*sde->head_dma = 0;

	reg = SD(ENG_ERR_CLEAR_SDMA_HEADER_REQUEST_FIFO_UNC_ERR_MASK) <<
	      SD(ENG_ERR_CLEAR_SDMA_HEADER_REQUEST_FIFO_UNC_ERR_SHIFT);
	write_sde_csr(sde, SD(ENG_ERR_CLEAR), reg);
}