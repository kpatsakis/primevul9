static void bnx2x_link_attn(struct bnx2x *bp)
{
	/* Make sure that we are synced with the current statistics */
	bnx2x_stats_handle(bp, STATS_EVENT_STOP);

	bnx2x_link_update(&bp->link_params, &bp->link_vars);

	bnx2x_init_dropless_fc(bp);

	if (bp->link_vars.link_up) {

		if (bp->link_vars.mac_type != MAC_TYPE_EMAC) {
			struct host_port_stats *pstats;

			pstats = bnx2x_sp(bp, port_stats);
			/* reset old mac stats */
			memset(&(pstats->mac_stx[0]), 0,
			       sizeof(struct mac_stx));
		}
		if (bp->state == BNX2X_STATE_OPEN)
			bnx2x_stats_handle(bp, STATS_EVENT_LINK_UP);
	}

	if (bp->link_vars.link_up && bp->link_vars.line_speed)
		bnx2x_set_local_cmng(bp);

	__bnx2x_link_report(bp);

	if (IS_MF(bp))
		bnx2x_link_sync_notify(bp);
}