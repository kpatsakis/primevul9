int bnx2x_configure_ptp_filters(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	int rc;

	if (!bp->hwtstamp_ioctl_called)
		return 0;

	switch (bp->tx_type) {
	case HWTSTAMP_TX_ON:
		bp->flags |= TX_TIMESTAMPING_EN;
		REG_WR(bp, port ? NIG_REG_P1_TLLH_PTP_PARAM_MASK :
		       NIG_REG_P0_TLLH_PTP_PARAM_MASK, 0x6AA);
		REG_WR(bp, port ? NIG_REG_P1_TLLH_PTP_RULE_MASK :
		       NIG_REG_P0_TLLH_PTP_RULE_MASK, 0x3EEE);
		break;
	case HWTSTAMP_TX_ONESTEP_SYNC:
		BNX2X_ERR("One-step timestamping is not supported\n");
		return -ERANGE;
	}

	switch (bp->rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		break;
	case HWTSTAMP_FILTER_ALL:
	case HWTSTAMP_FILTER_SOME:
	case HWTSTAMP_FILTER_NTP_ALL:
		bp->rx_filter = HWTSTAMP_FILTER_NONE;
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
	case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
	case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		bp->rx_filter = HWTSTAMP_FILTER_PTP_V1_L4_EVENT;
		/* Initialize PTP detection for UDP/IPv4 events */
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_PARAM_MASK :
		       NIG_REG_P0_LLH_PTP_PARAM_MASK, 0x7EE);
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_RULE_MASK :
		       NIG_REG_P0_LLH_PTP_RULE_MASK, 0x3FFE);
		break;
	case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		bp->rx_filter = HWTSTAMP_FILTER_PTP_V2_L4_EVENT;
		/* Initialize PTP detection for UDP/IPv4 or UDP/IPv6 events */
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_PARAM_MASK :
		       NIG_REG_P0_LLH_PTP_PARAM_MASK, 0x7EA);
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_RULE_MASK :
		       NIG_REG_P0_LLH_PTP_RULE_MASK, 0x3FEE);
		break;
	case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
		bp->rx_filter = HWTSTAMP_FILTER_PTP_V2_L2_EVENT;
		/* Initialize PTP detection L2 events */
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_PARAM_MASK :
		       NIG_REG_P0_LLH_PTP_PARAM_MASK, 0x6BF);
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_RULE_MASK :
		       NIG_REG_P0_LLH_PTP_RULE_MASK, 0x3EFF);

		break;
	case HWTSTAMP_FILTER_PTP_V2_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
		bp->rx_filter = HWTSTAMP_FILTER_PTP_V2_EVENT;
		/* Initialize PTP detection L2, UDP/IPv4 or UDP/IPv6 events */
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_PARAM_MASK :
		       NIG_REG_P0_LLH_PTP_PARAM_MASK, 0x6AA);
		REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_RULE_MASK :
		       NIG_REG_P0_LLH_PTP_RULE_MASK, 0x3EEE);
		break;
	}

	/* Indicate to FW that this PF expects recorded PTP packets */
	rc = bnx2x_enable_ptp_packets(bp);
	if (rc)
		return rc;

	/* Enable sending PTP packets to host */
	REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_TO_HOST :
	       NIG_REG_P0_LLH_PTP_TO_HOST, 0x1);

	return 0;
}