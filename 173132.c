static void bnx2x_handle_drv_info_req(struct bnx2x *bp)
{
	enum drv_info_opcode op_code;
	u32 drv_info_ctl = SHMEM2_RD(bp, drv_info_control);
	bool release = false;
	int wait;

	/* if drv_info version supported by MFW doesn't match - send NACK */
	if ((drv_info_ctl & DRV_INFO_CONTROL_VER_MASK) != DRV_INFO_CUR_VER) {
		bnx2x_fw_command(bp, DRV_MSG_CODE_DRV_INFO_NACK, 0);
		return;
	}

	op_code = (drv_info_ctl & DRV_INFO_CONTROL_OP_CODE_MASK) >>
		  DRV_INFO_CONTROL_OP_CODE_SHIFT;

	/* Must prevent other flows from accessing drv_info_to_mcp */
	mutex_lock(&bp->drv_info_mutex);

	memset(&bp->slowpath->drv_info_to_mcp, 0,
	       sizeof(union drv_info_to_mcp));

	switch (op_code) {
	case ETH_STATS_OPCODE:
		bnx2x_drv_info_ether_stat(bp);
		break;
	case FCOE_STATS_OPCODE:
		bnx2x_drv_info_fcoe_stat(bp);
		break;
	case ISCSI_STATS_OPCODE:
		bnx2x_drv_info_iscsi_stat(bp);
		break;
	default:
		/* if op code isn't supported - send NACK */
		bnx2x_fw_command(bp, DRV_MSG_CODE_DRV_INFO_NACK, 0);
		goto out;
	}

	/* if we got drv_info attn from MFW then these fields are defined in
	 * shmem2 for sure
	 */
	SHMEM2_WR(bp, drv_info_host_addr_lo,
		U64_LO(bnx2x_sp_mapping(bp, drv_info_to_mcp)));
	SHMEM2_WR(bp, drv_info_host_addr_hi,
		U64_HI(bnx2x_sp_mapping(bp, drv_info_to_mcp)));

	bnx2x_fw_command(bp, DRV_MSG_CODE_DRV_INFO_ACK, 0);

	/* Since possible management wants both this and get_driver_version
	 * need to wait until management notifies us it finished utilizing
	 * the buffer.
	 */
	if (!SHMEM2_HAS(bp, mfw_drv_indication)) {
		DP(BNX2X_MSG_MCP, "Management does not support indication\n");
	} else if (!bp->drv_info_mng_owner) {
		u32 bit = MFW_DRV_IND_READ_DONE_OFFSET((BP_ABS_FUNC(bp) >> 1));

		for (wait = 0; wait < BNX2X_UPDATE_DRV_INFO_IND_COUNT; wait++) {
			u32 indication = SHMEM2_RD(bp, mfw_drv_indication);

			/* Management is done; need to clear indication */
			if (indication & bit) {
				SHMEM2_WR(bp, mfw_drv_indication,
					  indication & ~bit);
				release = true;
				break;
			}

			msleep(BNX2X_UPDATE_DRV_INFO_IND_LENGTH);
		}
	}
	if (!release) {
		DP(BNX2X_MSG_MCP, "Management did not release indication\n");
		bp->drv_info_mng_owner = true;
	}

out:
	mutex_unlock(&bp->drv_info_mutex);
}