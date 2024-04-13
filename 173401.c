static void bnx2x_handle_afex_cmd(struct bnx2x *bp, u32 cmd)
{
	struct afex_stats afex_stats;
	u32 func = BP_ABS_FUNC(bp);
	u32 mf_config;
	u16 vlan_val;
	u32 vlan_prio;
	u16 vif_id;
	u8 allowed_prio;
	u8 vlan_mode;
	u32 addr_to_write, vifid, addrs, stats_type, i;

	if (cmd & DRV_STATUS_AFEX_LISTGET_REQ) {
		vifid = SHMEM2_RD(bp, afex_param1_to_driver[BP_FW_MB_IDX(bp)]);
		DP(BNX2X_MSG_MCP,
		   "afex: got MCP req LISTGET_REQ for vifid 0x%x\n", vifid);
		bnx2x_afex_handle_vif_list_cmd(bp, VIF_LIST_RULE_GET, vifid, 0);
	}

	if (cmd & DRV_STATUS_AFEX_LISTSET_REQ) {
		vifid = SHMEM2_RD(bp, afex_param1_to_driver[BP_FW_MB_IDX(bp)]);
		addrs = SHMEM2_RD(bp, afex_param2_to_driver[BP_FW_MB_IDX(bp)]);
		DP(BNX2X_MSG_MCP,
		   "afex: got MCP req LISTSET_REQ for vifid 0x%x addrs 0x%x\n",
		   vifid, addrs);
		bnx2x_afex_handle_vif_list_cmd(bp, VIF_LIST_RULE_SET, vifid,
					       addrs);
	}

	if (cmd & DRV_STATUS_AFEX_STATSGET_REQ) {
		addr_to_write = SHMEM2_RD(bp,
			afex_scratchpad_addr_to_write[BP_FW_MB_IDX(bp)]);
		stats_type = SHMEM2_RD(bp,
			afex_param1_to_driver[BP_FW_MB_IDX(bp)]);

		DP(BNX2X_MSG_MCP,
		   "afex: got MCP req STATSGET_REQ, write to addr 0x%x\n",
		   addr_to_write);

		bnx2x_afex_collect_stats(bp, (void *)&afex_stats, stats_type);

		/* write response to scratchpad, for MCP */
		for (i = 0; i < (sizeof(struct afex_stats)/sizeof(u32)); i++)
			REG_WR(bp, addr_to_write + i*sizeof(u32),
			       *(((u32 *)(&afex_stats))+i));

		/* send ack message to MCP */
		bnx2x_fw_command(bp, DRV_MSG_CODE_AFEX_STATSGET_ACK, 0);
	}

	if (cmd & DRV_STATUS_AFEX_VIFSET_REQ) {
		mf_config = MF_CFG_RD(bp, func_mf_config[func].config);
		bp->mf_config[BP_VN(bp)] = mf_config;
		DP(BNX2X_MSG_MCP,
		   "afex: got MCP req VIFSET_REQ, mf_config 0x%x\n",
		   mf_config);

		/* if VIF_SET is "enabled" */
		if (!(mf_config & FUNC_MF_CFG_FUNC_DISABLED)) {
			/* set rate limit directly to internal RAM */
			struct cmng_init_input cmng_input;
			struct rate_shaping_vars_per_vn m_rs_vn;
			size_t size = sizeof(struct rate_shaping_vars_per_vn);
			u32 addr = BAR_XSTRORM_INTMEM +
			    XSTORM_RATE_SHAPING_PER_VN_VARS_OFFSET(BP_FUNC(bp));

			bp->mf_config[BP_VN(bp)] = mf_config;

			bnx2x_calc_vn_max(bp, BP_VN(bp), &cmng_input);
			m_rs_vn.vn_counter.rate =
				cmng_input.vnic_max_rate[BP_VN(bp)];
			m_rs_vn.vn_counter.quota =
				(m_rs_vn.vn_counter.rate *
				 RS_PERIODIC_TIMEOUT_USEC) / 8;

			__storm_memset_struct(bp, addr, size, (u32 *)&m_rs_vn);

			/* read relevant values from mf_cfg struct in shmem */
			vif_id =
				(MF_CFG_RD(bp, func_mf_config[func].e1hov_tag) &
				 FUNC_MF_CFG_E1HOV_TAG_MASK) >>
				FUNC_MF_CFG_E1HOV_TAG_SHIFT;
			vlan_val =
				(MF_CFG_RD(bp, func_mf_config[func].e1hov_tag) &
				 FUNC_MF_CFG_AFEX_VLAN_MASK) >>
				FUNC_MF_CFG_AFEX_VLAN_SHIFT;
			vlan_prio = (mf_config &
				     FUNC_MF_CFG_TRANSMIT_PRIORITY_MASK) >>
				    FUNC_MF_CFG_TRANSMIT_PRIORITY_SHIFT;
			vlan_val |= (vlan_prio << VLAN_PRIO_SHIFT);
			vlan_mode =
				(MF_CFG_RD(bp,
					   func_mf_config[func].afex_config) &
				 FUNC_MF_CFG_AFEX_VLAN_MODE_MASK) >>
				FUNC_MF_CFG_AFEX_VLAN_MODE_SHIFT;
			allowed_prio =
				(MF_CFG_RD(bp,
					   func_mf_config[func].afex_config) &
				 FUNC_MF_CFG_AFEX_COS_FILTER_MASK) >>
				FUNC_MF_CFG_AFEX_COS_FILTER_SHIFT;

			/* send ramrod to FW, return in case of failure */
			if (bnx2x_afex_func_update(bp, vif_id, vlan_val,
						   allowed_prio))
				return;

			bp->afex_def_vlan_tag = vlan_val;
			bp->afex_vlan_mode = vlan_mode;
		} else {
			/* notify link down because BP->flags is disabled */
			bnx2x_link_report(bp);

			/* send INVALID VIF ramrod to FW */
			bnx2x_afex_func_update(bp, 0xFFFF, 0, 0);

			/* Reset the default afex VLAN */
			bp->afex_def_vlan_tag = -1;
		}
	}
}