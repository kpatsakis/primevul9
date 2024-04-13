static int bnx2x_set_storm_rx_mode(struct bnx2x *bp)
{
	unsigned long rx_mode_flags = 0, ramrod_flags = 0;
	unsigned long rx_accept_flags = 0, tx_accept_flags = 0;
	int rc;

	if (!NO_FCOE(bp))
		/* Configure rx_mode of FCoE Queue */
		__set_bit(BNX2X_RX_MODE_FCOE_ETH, &rx_mode_flags);

	rc = bnx2x_fill_accept_flags(bp, bp->rx_mode, &rx_accept_flags,
				     &tx_accept_flags);
	if (rc)
		return rc;

	__set_bit(RAMROD_RX, &ramrod_flags);
	__set_bit(RAMROD_TX, &ramrod_flags);

	return bnx2x_set_q_rx_mode(bp, bp->fp->cl_id, rx_mode_flags,
				   rx_accept_flags, tx_accept_flags,
				   ramrod_flags);
}