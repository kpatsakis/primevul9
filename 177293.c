static void sdma_sendctrl(struct sdma_engine *sde, unsigned op)
{
	u64 set_senddmactrl = 0;
	u64 clr_senddmactrl = 0;
	unsigned long flags;

#ifdef CONFIG_SDMA_VERBOSITY
	dd_dev_err(sde->dd, "CONFIG SDMA(%u) senddmactrl E=%d I=%d H=%d C=%d\n",
		   sde->this_idx,
		   (op & SDMA_SENDCTRL_OP_ENABLE) ? 1 : 0,
		   (op & SDMA_SENDCTRL_OP_INTENABLE) ? 1 : 0,
		   (op & SDMA_SENDCTRL_OP_HALT) ? 1 : 0,
		   (op & SDMA_SENDCTRL_OP_CLEANUP) ? 1 : 0);
#endif

	if (op & SDMA_SENDCTRL_OP_ENABLE)
		set_senddmactrl |= SD(CTRL_SDMA_ENABLE_SMASK);
	else
		clr_senddmactrl |= SD(CTRL_SDMA_ENABLE_SMASK);

	if (op & SDMA_SENDCTRL_OP_INTENABLE)
		set_senddmactrl |= SD(CTRL_SDMA_INT_ENABLE_SMASK);
	else
		clr_senddmactrl |= SD(CTRL_SDMA_INT_ENABLE_SMASK);

	if (op & SDMA_SENDCTRL_OP_HALT)
		set_senddmactrl |= SD(CTRL_SDMA_HALT_SMASK);
	else
		clr_senddmactrl |= SD(CTRL_SDMA_HALT_SMASK);

	spin_lock_irqsave(&sde->senddmactrl_lock, flags);

	sde->p_senddmactrl |= set_senddmactrl;
	sde->p_senddmactrl &= ~clr_senddmactrl;

	if (op & SDMA_SENDCTRL_OP_CLEANUP)
		write_sde_csr(sde, SD(CTRL),
			      sde->p_senddmactrl |
			      SD(CTRL_SDMA_CLEANUP_SMASK));
	else
		write_sde_csr(sde, SD(CTRL), sde->p_senddmactrl);

	spin_unlock_irqrestore(&sde->senddmactrl_lock, flags);

#ifdef CONFIG_SDMA_VERBOSITY
	sdma_dumpstate(sde);
#endif
}