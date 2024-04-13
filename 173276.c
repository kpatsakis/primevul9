static bool bnx2x_get_dropless_info(struct bnx2x *bp)
{
	int tmp;
	u32 cfg;

	if (IS_VF(bp))
		return false;

	if (IS_MF(bp) && !CHIP_IS_E1x(bp)) {
		/* Take function: tmp = func */
		tmp = BP_ABS_FUNC(bp);
		cfg = MF_CFG_RD(bp, func_ext_config[tmp].func_cfg);
		cfg = !!(cfg & MACP_FUNC_CFG_PAUSE_ON_HOST_RING);
	} else {
		/* Take port: tmp = port */
		tmp = BP_PORT(bp);
		cfg = SHMEM_RD(bp,
			       dev_info.port_hw_config[tmp].generic_features);
		cfg = !!(cfg & PORT_HW_CFG_PAUSE_ON_HOST_RING_ENABLED);
	}
	return cfg;
}