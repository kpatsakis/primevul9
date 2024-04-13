static void storm_memset_cmng(struct bnx2x *bp,
			      struct cmng_init *cmng,
			      u8 port)
{
	int vn;
	size_t size = sizeof(struct cmng_struct_per_port);

	u32 addr = BAR_XSTRORM_INTMEM +
			XSTORM_CMNG_PER_PORT_VARS_OFFSET(port);

	__storm_memset_struct(bp, addr, size, (u32 *)&cmng->port);

	for (vn = VN_0; vn < BP_MAX_VN_NUM(bp); vn++) {
		int func = func_by_vn(bp, vn);

		addr = BAR_XSTRORM_INTMEM +
		       XSTORM_RATE_SHAPING_PER_VN_VARS_OFFSET(func);
		size = sizeof(struct rate_shaping_vars_per_vn);
		__storm_memset_struct(bp, addr, size,
				      (u32 *)&cmng->vnic.vnic_max_rate[vn]);

		addr = BAR_XSTRORM_INTMEM +
		       XSTORM_FAIRNESS_PER_VN_VARS_OFFSET(func);
		size = sizeof(struct fairness_vars_per_vn);
		__storm_memset_struct(bp, addr, size,
				      (u32 *)&cmng->vnic.vnic_min_rate[vn]);
	}
}