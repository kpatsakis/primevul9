void bnx2x__link_status_update(struct bnx2x *bp)
{
	if (bp->state != BNX2X_STATE_OPEN)
		return;

	/* read updated dcb configuration */
	if (IS_PF(bp)) {
		bnx2x_dcbx_pmf_update(bp);
		bnx2x_link_status_update(&bp->link_params, &bp->link_vars);
		if (bp->link_vars.link_up)
			bnx2x_stats_handle(bp, STATS_EVENT_LINK_UP);
		else
			bnx2x_stats_handle(bp, STATS_EVENT_STOP);
			/* indicate link status */
		bnx2x_link_report(bp);

	} else { /* VF */
		bp->port.supported[0] |= (SUPPORTED_10baseT_Half |
					  SUPPORTED_10baseT_Full |
					  SUPPORTED_100baseT_Half |
					  SUPPORTED_100baseT_Full |
					  SUPPORTED_1000baseT_Full |
					  SUPPORTED_2500baseX_Full |
					  SUPPORTED_10000baseT_Full |
					  SUPPORTED_TP |
					  SUPPORTED_FIBRE |
					  SUPPORTED_Autoneg |
					  SUPPORTED_Pause |
					  SUPPORTED_Asym_Pause);
		bp->port.advertising[0] = bp->port.supported[0];

		bp->link_params.bp = bp;
		bp->link_params.port = BP_PORT(bp);
		bp->link_params.req_duplex[0] = DUPLEX_FULL;
		bp->link_params.req_flow_ctrl[0] = BNX2X_FLOW_CTRL_NONE;
		bp->link_params.req_line_speed[0] = SPEED_10000;
		bp->link_params.speed_cap_mask[0] = 0x7f0000;
		bp->link_params.switch_cfg = SWITCH_CFG_10G;
		bp->link_vars.mac_type = MAC_TYPE_BMAC;
		bp->link_vars.line_speed = SPEED_10000;
		bp->link_vars.link_status =
			(LINK_STATUS_LINK_UP |
			 LINK_STATUS_SPEED_AND_DUPLEX_10GTFD);
		bp->link_vars.link_up = 1;
		bp->link_vars.duplex = DUPLEX_FULL;
		bp->link_vars.flow_ctrl = BNX2X_FLOW_CTRL_NONE;
		__bnx2x_link_report(bp);

		bnx2x_sample_bulletin(bp);

		/* if bulletin board did not have an update for link status
		 * __bnx2x_link_report will report current status
		 * but it will NOT duplicate report in case of already reported
		 * during sampling bulletin board.
		 */
		bnx2x_stats_handle(bp, STATS_EVENT_LINK_UP);
	}
}