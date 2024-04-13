static unsigned long bnx2x_get_common_flags(struct bnx2x *bp,
					    struct bnx2x_fastpath *fp,
					    bool zero_stats)
{
	unsigned long flags = 0;

	/* PF driver will always initialize the Queue to an ACTIVE state */
	__set_bit(BNX2X_Q_FLG_ACTIVE, &flags);

	/* tx only connections collect statistics (on the same index as the
	 * parent connection). The statistics are zeroed when the parent
	 * connection is initialized.
	 */

	__set_bit(BNX2X_Q_FLG_STATS, &flags);
	if (zero_stats)
		__set_bit(BNX2X_Q_FLG_ZERO_STATS, &flags);

	if (bp->flags & TX_SWITCHING)
		__set_bit(BNX2X_Q_FLG_TX_SWITCH, &flags);

	__set_bit(BNX2X_Q_FLG_PCSUM_ON_PKT, &flags);
	__set_bit(BNX2X_Q_FLG_TUN_INC_INNER_IP_ID, &flags);

#ifdef BNX2X_STOP_ON_ERROR
	__set_bit(BNX2X_Q_FLG_TX_SEC, &flags);
#endif

	return flags;
}