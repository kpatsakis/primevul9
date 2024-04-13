irqreturn_t bnx2x_interrupt(int irq, void *dev_instance)
{
	struct bnx2x *bp = netdev_priv(dev_instance);
	u16 status = bnx2x_ack_int(bp);
	u16 mask;
	int i;
	u8 cos;

	/* Return here if interrupt is shared and it's not for us */
	if (unlikely(status == 0)) {
		DP(NETIF_MSG_INTR, "not our interrupt!\n");
		return IRQ_NONE;
	}
	DP(NETIF_MSG_INTR, "got an interrupt  status 0x%x\n", status);

#ifdef BNX2X_STOP_ON_ERROR
	if (unlikely(bp->panic))
		return IRQ_HANDLED;
#endif

	for_each_eth_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];

		mask = 0x2 << (fp->index + CNIC_SUPPORT(bp));
		if (status & mask) {
			/* Handle Rx or Tx according to SB id */
			for_each_cos_in_tx_queue(fp, cos)
				prefetch(fp->txdata_ptr[cos]->tx_cons_sb);
			prefetch(&fp->sb_running_index[SM_RX_ID]);
			napi_schedule_irqoff(&bnx2x_fp(bp, fp->index, napi));
			status &= ~mask;
		}
	}

	if (CNIC_SUPPORT(bp)) {
		mask = 0x2;
		if (status & (mask | 0x1)) {
			struct cnic_ops *c_ops = NULL;

			rcu_read_lock();
			c_ops = rcu_dereference(bp->cnic_ops);
			if (c_ops && (bp->cnic_eth_dev.drv_state &
				      CNIC_DRV_STATE_HANDLES_IRQ))
				c_ops->cnic_handler(bp->cnic_data, NULL);
			rcu_read_unlock();

			status &= ~mask;
		}
	}

	if (unlikely(status & 0x1)) {

		/* schedule sp task to perform default status block work, ack
		 * attentions and enable interrupts.
		 */
		bnx2x_schedule_sp_task(bp);

		status &= ~0x1;
		if (!status)
			return IRQ_HANDLED;
	}

	if (unlikely(status))
		DP(NETIF_MSG_INTR, "got an unknown interrupt! (status 0x%x)\n",
		   status);

	return IRQ_HANDLED;
}