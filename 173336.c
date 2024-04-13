static int bnx2x_shared_fcoe_funcs(struct bnx2x *bp)
{
	u8 count = 0;

	if (IS_MF(bp)) {
		u8 fid;

		/* iterate over absolute function ids for this path: */
		for (fid = BP_PATH(bp); fid < E2_FUNC_MAX * 2; fid += 2) {
			if (IS_MF_SD(bp)) {
				u32 cfg = MF_CFG_RD(bp,
						    func_mf_config[fid].config);

				if (!(cfg & FUNC_MF_CFG_FUNC_HIDE) &&
				    ((cfg & FUNC_MF_CFG_PROTOCOL_MASK) ==
					    FUNC_MF_CFG_PROTOCOL_FCOE))
					count++;
			} else {
				u32 cfg = MF_CFG_RD(bp,
						    func_ext_config[fid].
								      func_cfg);

				if ((cfg & MACP_FUNC_CFG_FLAGS_ENABLED) &&
				    (cfg & MACP_FUNC_CFG_FLAGS_FCOE_OFFLOAD))
					count++;
			}
		}
	} else { /* SF */
		int port, port_cnt = CHIP_MODE_IS_4_PORT(bp) ? 2 : 1;

		for (port = 0; port < port_cnt; port++) {
			u32 lic = SHMEM_RD(bp,
					   drv_lic_key[port].max_fcoe_conn) ^
				  FW_ENCODE_32BIT_PATTERN;
			if (lic)
				count++;
		}
	}

	return count;
}