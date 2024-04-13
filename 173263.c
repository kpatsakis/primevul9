void bnx2x_func_init(struct bnx2x *bp, struct bnx2x_func_init_params *p)
{
	if (CHIP_IS_E1x(bp)) {
		struct tstorm_eth_function_common_config tcfg = {0};

		storm_memset_func_cfg(bp, &tcfg, p->func_id);
	}

	/* Enable the function in the FW */
	storm_memset_vf_to_pf(bp, p->func_id, p->pf_id);
	storm_memset_func_en(bp, p->func_id, 1);

	/* spq */
	if (p->spq_active) {
		storm_memset_spq_addr(bp, p->spq_map, p->func_id);
		REG_WR(bp, XSEM_REG_FAST_MEMORY +
		       XSTORM_SPQ_PROD_OFFSET(p->func_id), p->spq_prod);
	}
}