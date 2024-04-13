static void init_sdma_regs(
	struct sdma_engine *sde,
	u32 credits,
	uint idle_cnt)
{
	u8 opval, opmask;
#ifdef CONFIG_SDMA_VERBOSITY
	struct hfi1_devdata *dd = sde->dd;

	dd_dev_err(dd, "CONFIG SDMA(%u) %s:%d %s()\n",
		   sde->this_idx, slashstrip(__FILE__), __LINE__, __func__);
#endif

	write_sde_csr(sde, SD(BASE_ADDR), sde->descq_phys);
	sdma_setlengen(sde);
	sdma_update_tail(sde, 0); /* Set SendDmaTail */
	write_sde_csr(sde, SD(RELOAD_CNT), idle_cnt);
	write_sde_csr(sde, SD(DESC_CNT), 0);
	write_sde_csr(sde, SD(HEAD_ADDR), sde->head_phys);
	write_sde_csr(sde, SD(MEMORY),
		      ((u64)credits << SD(MEMORY_SDMA_MEMORY_CNT_SHIFT)) |
		      ((u64)(credits * sde->this_idx) <<
		       SD(MEMORY_SDMA_MEMORY_INDEX_SHIFT)));
	write_sde_csr(sde, SD(ENG_ERR_MASK), ~0ull);
	set_sdma_integrity(sde);
	opmask = OPCODE_CHECK_MASK_DISABLED;
	opval = OPCODE_CHECK_VAL_DISABLED;
	write_sde_csr(sde, SD(CHECK_OPCODE),
		      (opmask << SEND_CTXT_CHECK_OPCODE_MASK_SHIFT) |
		      (opval << SEND_CTXT_CHECK_OPCODE_VALUE_SHIFT));
}