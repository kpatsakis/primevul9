static void storm_memset_func_cfg(struct bnx2x *bp,
				 struct tstorm_eth_function_common_config *tcfg,
				 u16 abs_fid)
{
	size_t size = sizeof(struct tstorm_eth_function_common_config);

	u32 addr = BAR_TSTRORM_INTMEM +
			TSTORM_FUNCTION_COMMON_CONFIG_OFFSET(abs_fid);

	__storm_memset_struct(bp, addr, size, (u32 *)tcfg);
}