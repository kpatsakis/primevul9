void bnx2x__init_func_obj(struct bnx2x *bp)
{
	/* Prepare DMAE related driver resources */
	bnx2x_setup_dmae(bp);

	bnx2x_init_func_obj(bp, &bp->func_obj,
			    bnx2x_sp(bp, func_rdata),
			    bnx2x_sp_mapping(bp, func_rdata),
			    bnx2x_sp(bp, func_afex_rdata),
			    bnx2x_sp_mapping(bp, func_afex_rdata),
			    &bnx2x_func_sp_drv);
}