static void bnx2x_drv_info_fcoe_stat(struct bnx2x *bp)
{
	struct bnx2x_dcbx_app_params *app = &bp->dcbx_port_params.app;
	struct fcoe_stats_info *fcoe_stat =
		&bp->slowpath->drv_info_to_mcp.fcoe_stat;

	if (!CNIC_LOADED(bp))
		return;

	memcpy(fcoe_stat->mac_local + MAC_PAD, bp->fip_mac, ETH_ALEN);

	fcoe_stat->qos_priority =
		app->traffic_type_priority[LLFC_TRAFFIC_TYPE_FCOE];

	/* insert FCoE stats from ramrod response */
	if (!NO_FCOE(bp)) {
		struct tstorm_per_queue_stats *fcoe_q_tstorm_stats =
			&bp->fw_stats_data->queue_stats[FCOE_IDX(bp)].
			tstorm_queue_statistics;

		struct xstorm_per_queue_stats *fcoe_q_xstorm_stats =
			&bp->fw_stats_data->queue_stats[FCOE_IDX(bp)].
			xstorm_queue_statistics;

		struct fcoe_statistics_params *fw_fcoe_stat =
			&bp->fw_stats_data->fcoe;

		ADD_64_LE(fcoe_stat->rx_bytes_hi, LE32_0,
			  fcoe_stat->rx_bytes_lo,
			  fw_fcoe_stat->rx_stat0.fcoe_rx_byte_cnt);

		ADD_64_LE(fcoe_stat->rx_bytes_hi,
			  fcoe_q_tstorm_stats->rcv_ucast_bytes.hi,
			  fcoe_stat->rx_bytes_lo,
			  fcoe_q_tstorm_stats->rcv_ucast_bytes.lo);

		ADD_64_LE(fcoe_stat->rx_bytes_hi,
			  fcoe_q_tstorm_stats->rcv_bcast_bytes.hi,
			  fcoe_stat->rx_bytes_lo,
			  fcoe_q_tstorm_stats->rcv_bcast_bytes.lo);

		ADD_64_LE(fcoe_stat->rx_bytes_hi,
			  fcoe_q_tstorm_stats->rcv_mcast_bytes.hi,
			  fcoe_stat->rx_bytes_lo,
			  fcoe_q_tstorm_stats->rcv_mcast_bytes.lo);

		ADD_64_LE(fcoe_stat->rx_frames_hi, LE32_0,
			  fcoe_stat->rx_frames_lo,
			  fw_fcoe_stat->rx_stat0.fcoe_rx_pkt_cnt);

		ADD_64_LE(fcoe_stat->rx_frames_hi, LE32_0,
			  fcoe_stat->rx_frames_lo,
			  fcoe_q_tstorm_stats->rcv_ucast_pkts);

		ADD_64_LE(fcoe_stat->rx_frames_hi, LE32_0,
			  fcoe_stat->rx_frames_lo,
			  fcoe_q_tstorm_stats->rcv_bcast_pkts);

		ADD_64_LE(fcoe_stat->rx_frames_hi, LE32_0,
			  fcoe_stat->rx_frames_lo,
			  fcoe_q_tstorm_stats->rcv_mcast_pkts);

		ADD_64_LE(fcoe_stat->tx_bytes_hi, LE32_0,
			  fcoe_stat->tx_bytes_lo,
			  fw_fcoe_stat->tx_stat.fcoe_tx_byte_cnt);

		ADD_64_LE(fcoe_stat->tx_bytes_hi,
			  fcoe_q_xstorm_stats->ucast_bytes_sent.hi,
			  fcoe_stat->tx_bytes_lo,
			  fcoe_q_xstorm_stats->ucast_bytes_sent.lo);

		ADD_64_LE(fcoe_stat->tx_bytes_hi,
			  fcoe_q_xstorm_stats->bcast_bytes_sent.hi,
			  fcoe_stat->tx_bytes_lo,
			  fcoe_q_xstorm_stats->bcast_bytes_sent.lo);

		ADD_64_LE(fcoe_stat->tx_bytes_hi,
			  fcoe_q_xstorm_stats->mcast_bytes_sent.hi,
			  fcoe_stat->tx_bytes_lo,
			  fcoe_q_xstorm_stats->mcast_bytes_sent.lo);

		ADD_64_LE(fcoe_stat->tx_frames_hi, LE32_0,
			  fcoe_stat->tx_frames_lo,
			  fw_fcoe_stat->tx_stat.fcoe_tx_pkt_cnt);

		ADD_64_LE(fcoe_stat->tx_frames_hi, LE32_0,
			  fcoe_stat->tx_frames_lo,
			  fcoe_q_xstorm_stats->ucast_pkts_sent);

		ADD_64_LE(fcoe_stat->tx_frames_hi, LE32_0,
			  fcoe_stat->tx_frames_lo,
			  fcoe_q_xstorm_stats->bcast_pkts_sent);

		ADD_64_LE(fcoe_stat->tx_frames_hi, LE32_0,
			  fcoe_stat->tx_frames_lo,
			  fcoe_q_xstorm_stats->mcast_pkts_sent);
	}

	/* ask L5 driver to add data to the struct */
	bnx2x_cnic_notify(bp, CNIC_CTL_FCOE_STATS_GET_CMD);
}