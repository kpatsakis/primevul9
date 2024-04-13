int bnx2x_issue_dmae_with_comp(struct bnx2x *bp, struct dmae_command *dmae,
			       u32 *comp)
{
	int cnt = CHIP_REV_IS_SLOW(bp) ? (400000) : 4000;
	int rc = 0;

	bnx2x_dp_dmae(bp, dmae, BNX2X_MSG_DMAE);

	/* Lock the dmae channel. Disable BHs to prevent a dead-lock
	 * as long as this code is called both from syscall context and
	 * from ndo_set_rx_mode() flow that may be called from BH.
	 */

	spin_lock_bh(&bp->dmae_lock);

	/* reset completion */
	*comp = 0;

	/* post the command on the channel used for initializations */
	bnx2x_post_dmae(bp, dmae, INIT_DMAE_C(bp));

	/* wait for completion */
	udelay(5);
	while ((*comp & ~DMAE_PCI_ERR_FLAG) != DMAE_COMP_VAL) {

		if (!cnt ||
		    (bp->recovery_state != BNX2X_RECOVERY_DONE &&
		     bp->recovery_state != BNX2X_RECOVERY_NIC_LOADING)) {
			BNX2X_ERR("DMAE timeout!\n");
			rc = DMAE_TIMEOUT;
			goto unlock;
		}
		cnt--;
		udelay(50);
	}
	if (*comp & DMAE_PCI_ERR_FLAG) {
		BNX2X_ERR("DMAE PCI error!\n");
		rc = DMAE_PCI_ERROR;
	}

unlock:

	spin_unlock_bh(&bp->dmae_lock);

	return rc;
}