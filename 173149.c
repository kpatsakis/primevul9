static void bnx2x_attn_int_deasserted3(struct bnx2x *bp, u32 attn)
{
	u32 val;

	if (attn & EVEREST_GEN_ATTN_IN_USE_MASK) {

		if (attn & BNX2X_PMF_LINK_ASSERT) {
			int func = BP_FUNC(bp);

			REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_12 + func*4, 0);
			bnx2x_read_mf_cfg(bp);
			bp->mf_config[BP_VN(bp)] = MF_CFG_RD(bp,
					func_mf_config[BP_ABS_FUNC(bp)].config);
			val = SHMEM_RD(bp,
				       func_mb[BP_FW_MB_IDX(bp)].drv_status);

			if (val & (DRV_STATUS_DCC_EVENT_MASK |
				   DRV_STATUS_OEM_EVENT_MASK))
				bnx2x_oem_event(bp,
					(val & (DRV_STATUS_DCC_EVENT_MASK |
						DRV_STATUS_OEM_EVENT_MASK)));

			if (val & DRV_STATUS_SET_MF_BW)
				bnx2x_set_mf_bw(bp);

			if (val & DRV_STATUS_DRV_INFO_REQ)
				bnx2x_handle_drv_info_req(bp);

			if (val & DRV_STATUS_VF_DISABLED)
				bnx2x_schedule_iov_task(bp,
							BNX2X_IOV_HANDLE_FLR);

			if ((bp->port.pmf == 0) && (val & DRV_STATUS_PMF))
				bnx2x_pmf_update(bp);

			if (bp->port.pmf &&
			    (val & DRV_STATUS_DCBX_NEGOTIATION_RESULTS) &&
				bp->dcbx_enabled > 0)
				/* start dcbx state machine */
				bnx2x_dcbx_set_params(bp,
					BNX2X_DCBX_STATE_NEG_RECEIVED);
			if (val & DRV_STATUS_AFEX_EVENT_MASK)
				bnx2x_handle_afex_cmd(bp,
					val & DRV_STATUS_AFEX_EVENT_MASK);
			if (val & DRV_STATUS_EEE_NEGOTIATION_RESULTS)
				bnx2x_handle_eee_event(bp);

			if (val & DRV_STATUS_OEM_UPDATE_SVID)
				bnx2x_handle_update_svid_cmd(bp);

			if (bp->link_vars.periodic_flags &
			    PERIODIC_FLAGS_LINK_EVENT) {
				/*  sync with link */
				bnx2x_acquire_phy_lock(bp);
				bp->link_vars.periodic_flags &=
					~PERIODIC_FLAGS_LINK_EVENT;
				bnx2x_release_phy_lock(bp);
				if (IS_MF(bp))
					bnx2x_link_sync_notify(bp);
				bnx2x_link_report(bp);
			}
			/* Always call it here: bnx2x_link_report() will
			 * prevent the link indication duplication.
			 */
			bnx2x__link_status_update(bp);
		} else if (attn & BNX2X_MC_ASSERT_BITS) {

			BNX2X_ERR("MC assert!\n");
			bnx2x_mc_assert(bp);
			REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_10, 0);
			REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_9, 0);
			REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_8, 0);
			REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_7, 0);
			bnx2x_panic();

		} else if (attn & BNX2X_MCP_ASSERT) {

			BNX2X_ERR("MCP assert!\n");
			REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_11, 0);
			bnx2x_fw_dump(bp);

		} else
			BNX2X_ERR("Unknown HW assert! (attn 0x%x)\n", attn);
	}

	if (attn & EVEREST_LATCHED_ATTN_IN_USE_MASK) {
		BNX2X_ERR("LATCHED attention 0x%08x (masked)\n", attn);
		if (attn & BNX2X_GRC_TIMEOUT) {
			val = CHIP_IS_E1(bp) ? 0 :
					REG_RD(bp, MISC_REG_GRC_TIMEOUT_ATTN);
			BNX2X_ERR("GRC time-out 0x%08x\n", val);
		}
		if (attn & BNX2X_GRC_RSV) {
			val = CHIP_IS_E1(bp) ? 0 :
					REG_RD(bp, MISC_REG_GRC_RSV_ATTN);
			BNX2X_ERR("GRC reserved 0x%08x\n", val);
		}
		REG_WR(bp, MISC_REG_AEU_CLR_LATCH_SIGNAL, 0x7ff);
	}
}